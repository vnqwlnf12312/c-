#include <iostream>

#include "iostream"
#include "memory"

template <typename T>
class EnableSharedFromThis;

namespace ControlBlock {
struct BaseControlBlock {
  int shared_count_ = 0;
  int weak_count_ = 0;
  BaseControlBlock() = default;
  virtual void useDeleter() = 0;
  virtual void destroyThis() = 0;
  virtual ~BaseControlBlock() = default;
};
template <typename U, typename Alloc>
struct ControlBlockAllocateShared : BaseControlBlock {
  Alloc alloc_;
  alignas(U) char elem_[sizeof(U)];
  template <typename... Args>
  ControlBlockAllocateShared(Alloc alloc, Args&&... args);
  void destroyThis() override;
  void useDeleter() override;
};
template <typename U, typename Alloc, typename Deleter>
struct ControlBlockRegular : BaseControlBlock {
  Deleter deleter_;
  Alloc alloc_;
  U* elem_ = nullptr;
  ControlBlockRegular() = default;
  ControlBlockRegular(U* elem, Alloc alloc = std::allocator<U>(),
                      Deleter deleter = std::default_delete<U>());
  void destroyThis() override;
  void useDeleter() override;
};
}  // namespace ControlBlock

template <typename T>
class SharedPtr {
 public:
  template <typename U>
  friend class WeakPtr;
  template <typename U>
  friend class SharedPtr;
  SharedPtr() = default;
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  explicit SharedPtr(U* elem);
  SharedPtr(const SharedPtr& other);
  SharedPtr(SharedPtr&& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  SharedPtr(const SharedPtr<U>& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  SharedPtr(SharedPtr<U>&& other);
  SharedPtr& operator=(const SharedPtr& other);
  SharedPtr& operator=(SharedPtr&& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  SharedPtr& operator=(const SharedPtr<U>& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  SharedPtr& operator=(SharedPtr<U>&& other);
  template <typename Y, typename Deleter>
  SharedPtr(Y* elem, const Deleter& deleter);
  template <typename Y, typename Deleter, typename Allocator>
  SharedPtr(Y* elem, const Deleter& deleter, const Allocator& allocator);
  int use_count() const;
  void reset();
  template <typename Y>
  void reset(Y* elem);
  void swap(SharedPtr& other);
  T* get() const;
  T& operator*();
  const T& operator*() const;
  T* operator->();
  const T* operator->() const;
  ~SharedPtr();
  template <typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> allocateShared(Alloc alloc, Args&&... args);

 private:
  void clear();
  void construct(ControlBlock::BaseControlBlock* block, T* elem);
  template <typename U, typename Alloc>
  SharedPtr(ControlBlock::ControlBlockAllocateShared<U, Alloc>* block);
  SharedPtr(ControlBlock::BaseControlBlock* block, T* elem);
  ControlBlock::BaseControlBlock* block_ = nullptr;
  T* elem_ = nullptr;
};

template <typename U, typename Alloc>
template <typename... Args>
ControlBlock::ControlBlockAllocateShared<U, Alloc>::ControlBlockAllocateShared(
    Alloc alloc, Args&&... args)
    : alloc_(alloc) {
  new (reinterpret_cast<U*>(elem_)) U(std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc alloc, Args&&... args) {
  using BlockAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<
          typename ControlBlock::template ControlBlockAllocateShared<T, Alloc>>;
  BlockAlloc block_alloc = alloc;
  auto cb = std::allocator_traits<BlockAlloc>::allocate(block_alloc, 1);
  std::allocator_traits<BlockAlloc>::construct(block_alloc, cb, alloc,
                                               std::forward<Args>(args)...);
  return SharedPtr<T>(cb);
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T, std::allocator<T>, Args...>(
      std::allocator<T>(), std::forward<Args>(args)...);
}

template <typename T>
template <typename U, typename Alloc>
SharedPtr<T>::SharedPtr(
    ControlBlock::ControlBlockAllocateShared<U, Alloc>* block)
    : block_(block), elem_(reinterpret_cast<T*>(&block->elem_)) {
  if (block_) {
    ++block_->shared_count_;
  }
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    elem_->weak_ptr_ = *this;
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(ControlBlock::BaseControlBlock* block, T* elem)
    : block_(block), elem_(elem) {
  if (block_) {
    ++block_->shared_count_;
  }
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    if (!elem_) {
      throw std::bad_weak_ptr();
    }
    elem_->weak_ptr_ = *this;
  }
}

template <typename U, typename Alloc, typename Deleter>
ControlBlock::template ControlBlockRegular<
    U, Alloc, Deleter>::ControlBlockRegular(U* elem, Alloc alloc,
                                            Deleter deleter)
    : deleter_(deleter), alloc_(alloc), elem_(elem) {}

template <typename T>
template <typename U, typename enable_if>
SharedPtr<T>::SharedPtr(U* elem)
    : block_(
          new ControlBlock::ControlBlockRegular<U, std::allocator<U>,
                                                std::default_delete<U>>(elem)),
      elem_(elem) {
  static_assert(std::is_base_of_v<T, U> || std::is_same_v<T, U>);
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    elem->weak_pointer_ = this;
  }
  if (block_) {
    ++block_->shared_count_;
  }
}

template <typename T>
void SharedPtr<T>::clear() {
  if (!block_) {
    return;
  }
  --block_->shared_count_;
  if (block_->shared_count_ == 0) {
    if (block_->weak_count_ == 0) {
      block_->useDeleter();
      block_->destroyThis();
      block_ = nullptr;
      elem_ = nullptr;
    } else {
      block_->useDeleter();
    }
  }
}

template <typename T>
void SharedPtr<T>::construct(ControlBlock::BaseControlBlock* block, T* elem) {
  if (block_) {
    clear();
  }
  block_ = block;
  elem_ = elem;
}

template <typename U, typename Alloc>
void ControlBlock::ControlBlockAllocateShared<U, Alloc>::useDeleter() {
  using UAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<U>;
  UAlloc ualloc = alloc_;
  std::allocator_traits<UAlloc>::destroy(ualloc, reinterpret_cast<U*>(&elem_));
}

template <typename U, typename Alloc>
void ControlBlock::ControlBlockAllocateShared<U, Alloc>::destroyThis() {
  using SharedAlloc = typename std::allocator_traits<
      Alloc>::template rebind_alloc<ControlBlockAllocateShared<U, Alloc>>;
  SharedAlloc shared_alloc = alloc_;
  std::allocator_traits<SharedAlloc>::deallocate(shared_alloc, this, 1);
}

template <typename U, typename Alloc, typename Deleter>
void ControlBlock::ControlBlockRegular<U, Alloc, Deleter>::useDeleter() {
  deleter_(elem_);
}

template <typename U, typename Alloc, typename Deleter>
void ControlBlock::ControlBlockRegular<U, Alloc, Deleter>::destroyThis() {
  using RegularAlloc = typename std::allocator_traits<
      Alloc>::template rebind_alloc<ControlBlockRegular<U, Alloc, Deleter>>;
  RegularAlloc regular_alloc = alloc_;
  std::allocator_traits<RegularAlloc>::deallocate(regular_alloc, this, 1);
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
    : block_(other.block_), elem_(other.elem_) {
  if (block_) {
    ++block_->shared_count_;
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other)
    : block_(other.block_), elem_(other.elem_) {
  other.block_ = nullptr;
  other.elem_ = nullptr;
}

template <typename T>
template <typename U, typename enable_if>
SharedPtr<T>::SharedPtr(const SharedPtr<U>& other)
    : block_(other.block_), elem_(other.elem_) {
  static_assert(std::is_base_of_v<T, U> || std::is_same_v<T, U>);
  if (block_) {
    ++(block_->shared_count_);
  }
}

template <typename T>
template <typename U, typename enable_if>
SharedPtr<T>::SharedPtr(SharedPtr<U>&& other)
    : block_(other.block_), elem_(other.elem_) {
  static_assert(std::is_base_of_v<T, U> || std::is_same_v<T, U>);
  other.block_ = nullptr;
  other.elem_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.block_, other.elem_);
  if (block_) {
    ++block_->shared_count_;
  }
  return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.block_, other.elem_);
  other.block_ = nullptr;
  other.elem_ = nullptr;
  return *this;
}

template <typename T>
template <typename U, typename enable_if>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& other) {
  static_assert(std::is_base_of_v<T, U> || std::is_same_v<T, U>);
  if (this == reinterpret_cast<const SharedPtr<T>*>(&other)) {
    return *this;
  }
  construct(other.block_, other.elem_);
  if (block_) {
    ++block_->shared_count_;
  }
  return *this;
}

template <typename T>
template <typename U, typename enable_if>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) {
  static_assert(std::is_base_of_v<T, U> || std::is_same_v<T, U>);
  if (this == reinterpret_cast<SharedPtr<T>*>(&other)) {
    return *this;
  }
  construct(other.block_, other.elem_);
  other.block_ = nullptr;
  other.elem_ = nullptr;
  return *this;
}

template <typename T>
template <typename Y, typename Deleter>
SharedPtr<T>::SharedPtr(Y* elem, const Deleter& deleter) {
  auto* block =
      new ControlBlock::ControlBlockRegular<Y, std::allocator<Y>, Deleter>(
          elem, std::allocator<Y>(), deleter);
  block_ = block;
  elem_ = block->elem_;
  ++block_->shared_count_;
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    elem->weak_pointer_ = this;
  }
}

template <typename T>
template <typename Y, typename Deleter, typename Alloc>
SharedPtr<T>::SharedPtr(Y* elem, const Deleter& deleter, const Alloc& alloc) {
  using BlockAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<
          typename ControlBlock::template ControlBlockRegular<Y, Alloc,
                                                              Deleter>>;
  BlockAlloc block_alloc = alloc;
  auto* block = std::allocator_traits<BlockAlloc>::allocate(block_alloc, 1);
  new (block) ControlBlock::ControlBlockRegular<Y, Alloc, Deleter>(elem, alloc,
                                                                   deleter);
  block_ = block;
  elem_ = block->elem_;
  ++block_->shared_count_;
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    elem->weak_pointer_ = this;
  }
}

template <typename T>
int SharedPtr<T>::use_count() const {
  return block_->shared_count_;
}

template <typename T>
void SharedPtr<T>::reset() {
  clear();
  block_ = nullptr;
  elem_ = nullptr;
}

template <typename T>
template <typename Y>
void SharedPtr<T>::reset(Y* elem) {
  clear();
  auto* block =
      new ControlBlock::ControlBlockRegular<Y, std::allocator<Y>,
                                            std::default_delete<Y>>(elem);
  ++block->shared_count_;
  block_ = block;
  elem_ = block->elem_;
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr& other) {
  std::swap(elem_, other.elem_);
  std::swap(block_, other.block_);
}

template <typename T>
T* SharedPtr<T>::get() const {
  return elem_;
}

template <typename T>
T& SharedPtr<T>::operator*() {
  return *elem_;
}

template <typename T>
const T& SharedPtr<T>::operator*() const {
  return *elem_;
}

template <typename T>
T* SharedPtr<T>::operator->() {
  return elem_;
}

template <typename T>
const T* SharedPtr<T>::operator->() const {
  return elem_;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
  clear();
}

template <typename T>
class WeakPtr {
 public:
  template <typename U>
  friend class WeakPtr;
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  WeakPtr(const SharedPtr<U>& sharedptr);
  WeakPtr() = default;
  WeakPtr(const WeakPtr& other);
  WeakPtr(WeakPtr&& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  WeakPtr(const WeakPtr<U>& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  WeakPtr(WeakPtr<U>&& other);
  WeakPtr<T>& operator=(const WeakPtr& other);
  WeakPtr<T>& operator=(WeakPtr&& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  WeakPtr<T>& operator=(const WeakPtr<U>& other);
  template <typename U, typename = std::enable_if_t<std::is_same_v<T, U> ||
                                                    std::is_base_of_v<T, U>>>
  WeakPtr<T>& operator=(WeakPtr<U>&& other);
  bool expired() const;
  SharedPtr<T> lock() const;
  int use_count() const;
  ~WeakPtr();

 private:
  void clear();
  void construct(T* elem, typename ControlBlock::BaseControlBlock* block);
  typename ControlBlock::BaseControlBlock* block_ = nullptr;
  T* elem_ = nullptr;
};

template <typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& other)
    : block_(other.block_), elem_(other.elem_) {
  if (block_) {
    ++block_->weak_count_;
  }
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& other)
    : block_(other.block_), elem_(other.elem_) {
  other.block_ = nullptr;
  other.elem_ = nullptr;
}

template <typename T>
template <typename U, typename enable_if>
WeakPtr<T>::WeakPtr(const WeakPtr<U>& other)
    : block_(other.block_), elem_(other.elem_) {
  if (block_) {
    ++block_->weak_count_;
  }
}

template <typename T>
template <typename U, typename enable_if>
WeakPtr<T>::WeakPtr(WeakPtr<U>&& other)
    : block_(other.block_), elem_(other.elem_) {
  other.block_ = nullptr;
  other.elem_ = nullptr;
}

template <typename T>
template <typename U, typename enable_if>
WeakPtr<T>::WeakPtr(const SharedPtr<U>& sharedptr)
    : block_(sharedptr.block_), elem_(sharedptr.elem_) {
  if (block_) {
    ++block_->weak_count_;
  }
}

template <typename T>
void WeakPtr<T>::construct(T* elem,
                           typename ControlBlock::BaseControlBlock* block) {
  if (block_) {
    clear();
  }
  block_ = block;
  elem_ = elem;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.elem_, other.block_);
  if (block_) {
    ++block_->weak_count_;
  }
  return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.elem_, other.block_);
  other.block_ = nullptr;
  other.elem_ = nullptr;
  return *this;
}

template <typename T>
template <typename U, typename enable_if>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<U>& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.elem_, other.block_);
  if (block_) {
    ++block_->weak_count_;
  }
  return *this;
}

template <typename T>
template <typename U, typename enable_if>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& other) {
  if (this == &other) {
    return *this;
  }
  construct(other.elem_, other.block_);
  other.block_ = nullptr;
  other.elem_ = nullptr;
  return *this;
}

template <typename T>
bool WeakPtr<T>::expired() const {
  return block_->shared_count_ == 0;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const {
  return SharedPtr<T>(block_, elem_);
}

template <typename T>
int WeakPtr<T>::use_count() const {
  return block_->shared_count_;
}

template <typename T>
void WeakPtr<T>::clear() {
  if (!block_) {
    return;
  }
  --block_->weak_count_;
  if (block_->shared_count_ == 0 && block_->weak_count_ == 0) {
    block_->destroyThis();
    block_ = nullptr;
    elem_ = nullptr;
  }
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
  clear();
}

template <typename T>
class EnableSharedFromThis {
  template <typename U>
  friend class SharedPtr;
  SharedPtr<T> shared_from_this() const;

 private:
  WeakPtr<T> weak_ptr_;
};

template <typename T>
SharedPtr<T> EnableSharedFromThis<T>::shared_from_this() const {
  return weak_ptr_.lock();
}
