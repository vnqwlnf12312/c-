#include <iostream>
#include "memory"

template<size_t N>
class StackStorage {
 public:
  char arr_[N];
  size_t remained_space = 0;
  StackStorage() = default;
  StackStorage(const StackStorage& other) = delete;
  ~StackStorage();
};

template<typename T, size_t N>
class StackAllocator {
 public:
  StackAllocator();
  StackAllocator(StackStorage<N>& storage);

  template<typename U, size_t M>
  friend class StackAllocator;

  using value_type = T;
  using pointer = value_type*;
  using difference_type = int;
  using reference = value_type&;
  using iterator_category = std::bidirectional_iterator_tag;

  T* allocate(size_t n);
  void deallocate(T* pointer, size_t n);
  template<typename... Args>
  void construct(T* pointer, const Args&... args);
  void destroy(T* pointer);
  template<typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& other);

  template<typename U>
  StackAllocator& operator=(const StackAllocator<U, N>& other);
  ~StackAllocator();
 private:
  StackStorage<N>* storage_;
};

template<typename T, class Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;
    BaseNode(BaseNode* prev, BaseNode* next);
    ~BaseNode() = default;
  };
  struct Node : BaseNode {
    T value;
    Node(BaseNode* prev, BaseNode* next);
    Node(BaseNode* prev, BaseNode* next, const T& value);
    ~Node() = default;
  };
  using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  BaseNode end_;
  size_t size_;
  NodeAlloc node_alloc_;
  void clear();
  template<bool has_default_const>
  void construct(bool from_int, size_t size, const T* elem, const List* other);
 public:
  List();
  List(size_t size);
  List(size_t size, const T& elem);
  List(const Allocator& alloc);
  List(size_t size, const Allocator& alloc);
  List(size_t size, const T& elem, const Allocator& alloc);
  List(const List<T, Allocator>& other);
  List& operator=(const List<T, Allocator>& other);
  ~List();

  NodeAlloc & get_allocator();
  size_t size() const;
  void push_back(const T& elem);
  void push_front(const T& elem);
  void pop_back();
  void pop_front();

  template<typename U>
  class iterator_ {
   public:
    friend List;
    using value_type = U;
    using pointer = value_type*;
    using difference_type = int;
    using reference = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;
    iterator_();
    iterator_(const iterator_<value_type>& other);
    iterator_(BaseNode* node);
    operator iterator_<const value_type>() const;
    iterator_& operator=(const iterator_<value_type>& other);
    iterator_<value_type>& operator++();
    iterator_<value_type> operator++(int);
    iterator_<value_type>& operator--();
    iterator_<value_type> operator--(int);
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const iterator_<U>& other) const;
    bool operator!=(const iterator_<U>& other) const;
   private:
    BaseNode* cur_node_;
  };
  using iterator = iterator_<T>;
  using const_iterator = iterator_<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;
  void insert(const_iterator it, const T& elem);
  void erase(const_iterator it);
};

template<size_t N>
StackStorage<N>::~StackStorage() {}

template<typename T, size_t N>
StackAllocator<T, N>::StackAllocator() : storage_() {
}

template<typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& storage) : storage_(&storage) {}

template<typename T, size_t N>
template<typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& other) : storage_(other.storage_) {}

template<typename T, size_t N>
template<typename U>
StackAllocator<T,   N>& StackAllocator<T, N>::operator=(const StackAllocator<U, N>& other) {
  storage_ = other.storage_;
  return *this;
}

template<typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t n) {
  size_t remained_space = N - storage_->remained_space;
  void* cur_space = static_cast<void*>(storage_->arr_ + storage_->remained_space);
  if (std::align(alignof(T), sizeof(T) * n, cur_space, remained_space)) {
    T* to_return = reinterpret_cast<T*>(cur_space);
    storage_->remained_space = N - (remained_space - n * sizeof(T));
    return to_return;
  }
  throw std::bad_alloc();
}

template<typename T, size_t N>
void StackAllocator<T, N>::deallocate(T* pointer, size_t n) {
  std::ignore = pointer;
  std::ignore = n;
}

template<typename T, size_t N>
template<typename... Args>
void StackAllocator<T, N>::construct(T* pointer, const Args& ...args) {
  new (pointer) T(args...);
}

template<typename T, size_t N>
void StackAllocator<T, N>::destroy(T* pointer) {
  pointer->~T();
}

template<typename T, size_t N>
StackAllocator<T, N>::~StackAllocator() {}

//-----------------------------------------------------------------------

template<typename T, class Allocator>
List<T, Allocator>::BaseNode::BaseNode(List::BaseNode* prev, List::BaseNode* next) : prev(prev), next(next) {}

template<typename T, class Allocator>
List<T, Allocator>::Node::Node(List<T, Allocator>::BaseNode* prev, List<T, Allocator>::BaseNode* next) : BaseNode(prev, next), value() {}

template<typename T, class Allocator>
List<T, Allocator>::Node::Node(List<T, Allocator>::BaseNode* prev, List<T, Allocator>::BaseNode* next, const T& value) : BaseNode(prev, next), value(value){}

template<typename T, class Allocator>
List<T, Allocator>::List() : end_(BaseNode{&end_, &end_}), size_(0), node_alloc_(Allocator()) {}

template<typename T, class Allocator>
void List<T, Allocator>::clear() {
  while (end_.prev != &end_) {
    pop_back();
  }
}

template<typename T, class Allocator>
template<bool has_default_constructor>
void List<T, Allocator>::construct(bool from_int, size_t size, const T* elem, const List* other) {
  if (size == 0) {
    return;
  }
  try {
    if (from_int) {
      if constexpr (has_default_constructor) {
        for (size_t i = 0; i < size; ++i) {
          Node* new_node = std::allocator_traits<NodeAlloc>::allocate(node_alloc_, 1);
          std::allocator_traits<NodeAlloc>::construct(node_alloc_, new_node, end_.prev, &end_);
          end_.prev->next = static_cast<BaseNode*>(new_node);
          end_.prev = static_cast<BaseNode*>(new_node);
        }
      } else {
        for (size_t i = 0; i < size; ++i) {
          insert(iterator(&end_), *elem);
        }
      }
    } else {
      BaseNode* other_node = other->end_.next;
      for (size_t i = 0; i < size; ++i) {
        insert(iterator(&end_), static_cast<Node*>(other_node)->value);
        other_node = other_node->next;
      }
    }
  } catch (...) {
    clear();
  }
  size_ = size;
}

template<typename T, class Allocator>
List<T, Allocator>::List(size_t size) : List() {
  construct<true>(true, size, nullptr, nullptr);
}

template<typename T, class Allocator>
List<T, Allocator>::List(size_t size, const T& elem) : List() {
  construct<false>(true, size, &elem, nullptr);
}

template<typename T, class Allocator>
List<T, Allocator>::List(const Allocator& alloc) : end_(BaseNode{&end_, &end_}), size_(0), node_alloc_(alloc) {}

template<typename T, class Allocator>
List<T, Allocator>::List(size_t size, const Allocator& alloc) : List(alloc) {
  construct<true>(true, size, nullptr, nullptr);
}

template<typename T, class Allocator>
List<T, Allocator>::List(size_t size, const T& elem, const Allocator& alloc) : List(size, alloc) {
  construct<false>(size, &elem);
}

template<typename T, class Allocator>
typename List<T, Allocator>::NodeAlloc& List<T, Allocator>::get_allocator() {
  return node_alloc_;
}

template<typename T, class Allocator>
List<T, Allocator>::List(const List<T, Allocator>& other) : end_(BaseNode{&end_, &end_}), size_(0) {
  node_alloc_ = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.node_alloc_);
  construct<false>(false, other.size_, nullptr, &other);
}

template<typename T, class Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& other) {
  clear();
  if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
    node_alloc_ = other.node_alloc_;
  }
  construct<false>(false, other.size_, nullptr, &other);
  return *this;
}

template<typename T, class Allocator>
size_t List<T, Allocator>::size() const {
  return size_;
}

template<typename T, class Allocator>
void List<T, Allocator>::push_back(const T& elem) {
  insert(iterator(&end_), elem);
}

template<typename T, class Allocator>
void List<T, Allocator>::pop_back() {
  erase(--iterator(&end_));
}

template<typename T, class Allocator>
void List<T, Allocator>::push_front(const T& elem) {
  insert(iterator(end_.next), elem);
}

template<typename T, class Allocator>
void List<T, Allocator>::pop_front() {
  erase(iterator(end_.next));
}

template<typename T, class Allocator>
List<T, Allocator>::~List() {
  clear();
}

template<typename T, class Allocator>
template<typename U>
List<T, Allocator>::iterator_<U>::iterator_() : cur_node_(nullptr) {}

template<typename T, class Allocator>
template<typename U>
List<T, Allocator>::iterator_<U>::iterator_(const List<T, Allocator>::iterator_<U>& other) : cur_node_(other.cur_node_) {}

template<typename T, class Allocator>
template<typename U>
List<T, Allocator>::iterator_<U>::iterator_(BaseNode* node): cur_node_(node) {}

template<typename T, class Allocator>
template<typename U>
typename List<T, Allocator>::template iterator_<U>& List<T, Allocator>::iterator_<U>::operator=(const iterator_<U>& other) {
  cur_node_ = other.cur_node_;
  return *this;
}

template<typename T, class Allocator>
template<typename U>
typename List<T, Allocator>::template iterator_<U>& List<T, Allocator>::iterator_<U>::operator++() {
  cur_node_ = cur_node_->next;
  return *this;
}

template<typename T, class Allocator>
template<typename U>
typename List<T, Allocator>::template iterator_<U> List<T, Allocator>::iterator_<U>::operator++(int) {
  iterator to_return = iterator(cur_node_);
  cur_node_ = cur_node_->next;
  return to_return;
}

template<typename T, class Allocator>
template<typename U>
typename List<T, Allocator>::template iterator_<U>& List<T, Allocator>::iterator_<U>::operator--() {
  cur_node_ = cur_node_->prev;
  return *this;
}

template<typename T, class Allocator>
template<typename U>
typename List<T, Allocator>::template iterator_<U> List<T, Allocator>::iterator_<U>::operator--(int) {
  iterator to_return = iterator(cur_node_);
  cur_node_ = cur_node_->prev;
  return to_return;
}

template<typename T, class Allocator>
template<typename U>
U& List<T, Allocator>::iterator_<U>::operator*() const {
  return static_cast<Node*>(cur_node_)->value;
}

template<typename T, class Allocator>
template<typename U>
U* List<T, Allocator>::iterator_<U>::operator->() const {
  return &static_cast<Node*>(cur_node_)->value;
}

template<typename T, class Allocator>
template<typename U>
bool List<T, Allocator>::iterator_<U>::operator==(const iterator_<U>& other) const {
  return cur_node_ == other.cur_node_;
}

template<typename T, class Allocator>
template<typename U>
bool List<T, Allocator>::iterator_<U>::operator!=(const iterator_<U>& other) const {
  return !(*this == other);
}

template<typename T, class Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
  return iterator(end_.next);
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
  return const_iterator(end_.next);
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
  return const_iterator(end_.next);
}

template<typename T, class Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
  return iterator(&end_);
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
  return const_iterator(end_.next->prev);
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
  return const_iterator(&end_);
}

template<typename T, class Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
  return std::reverse_iterator(end());
}

template<typename T, class Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() {
  return std::reverse_iterator(begin());
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const{
  return std::reverse_iterator(end());
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const{
  return std::reverse_iterator(begin());
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const{
  return std::reverse_iterator(List<T, Allocator>::const_iterator(end_.next));
}

template<typename T, class Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const{
  return std::reverse_iterator(List<T, Allocator>::const_iterator(&end_));
}

template<typename T, class Allocator>
template<typename U>
List<T, Allocator>::iterator_<U>::operator iterator_<const U>() const {
  List<T, Allocator>::iterator_<const U> to_return;
  to_return.cur_node_ = cur_node_;
  return to_return;
}

template<typename T, class Allocator>
void List<T, Allocator>::insert(List<T, Allocator>::const_iterator it, const T& elem) {
  Node* new_node;
  BaseNode* prev = it.cur_node_->prev;
  try {
    new_node = std::allocator_traits<NodeAlloc>::allocate(node_alloc_, 1);
    std::allocator_traits<NodeAlloc>::construct(node_alloc_, new_node, prev, it.cur_node_, elem);
    prev->next = static_cast<BaseNode*>(new_node);
    it.cur_node_->prev = static_cast<BaseNode*>(new_node);
  } catch (...) {
    if (new_node == nullptr) {
      return;
    }
    it.cur_node_->prev = prev;
    prev->next = it.cur_node_;
  }
  ++size_;
}

template<typename T, class Allocator>
void List<T, Allocator>::erase(List<T, Allocator>::const_iterator it) {
  try {
    it.cur_node_->prev->next = it.cur_node_->next;
    it.cur_node_->next->prev = it.cur_node_->prev;
    std::allocator_traits<NodeAlloc>::destroy(node_alloc_, static_cast<Node*>(it.cur_node_));
    std::allocator_traits<NodeAlloc>::deallocate(node_alloc_, static_cast<Node*>(it.cur_node_), 1);
  } catch (...) {
    return;
  }
  --size_;
}
