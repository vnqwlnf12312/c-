#include "stdexcept"
#include "vector"

template <typename Key, typename Value, typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
  using NodeTypeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType>;

 private:
  template <typename T, class Allocator = std::allocator<T>>
  class List {
   private:
    struct BaseNode {
      friend UnorderedMap;
      BaseNode* prev;
      BaseNode* next;
      size_t hash = 0;
      BaseNode(BaseNode* prev, BaseNode* next);
      explicit BaseNode(BaseNode&& other);
      BaseNode& operator=(BaseNode&& other);
      ~BaseNode() = default;
    };
    struct Node : BaseNode {
      T value;
      Node(BaseNode* prev, BaseNode* next);
      Node(BaseNode* prev, BaseNode* next, const T& value);
      ~Node() = default;
    };
    using NodeAlloc =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    BaseNode end_;
    size_t size_;
    NodeAlloc node_alloc_;

    template <bool has_default_const>
    void construct(bool from_int, size_t size, const T* elem,
                   const List* other);

   public:
    friend UnorderedMap;

    List();
    explicit List(size_t size);
    List(size_t size, const T& elem);
    explicit List(const Allocator& alloc);
    List(size_t size, const Allocator& alloc);
    List(size_t size, const T& elem, const Allocator& alloc);
    explicit List(const List<T, Allocator>& other);
    explicit List(List<T, Allocator>&& other);
    List& operator=(const List<T, Allocator>& other);
    List& operator=(List<T, Allocator>&& other);
    ~List();

    NodeAlloc& get_allocator();
    size_t size() const;
    void push_back(const T& elem);
    void push_front(const T& elem);
    void pop_back();
    void pop_front();
    void clear();

    template <typename U>
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

      BaseNode* getNode();

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

    template <typename... Args>
    void emplace(Node* node, Args&&... args);

    void insert(const_iterator it, const T& elem);
    void insertNode(const_iterator it, BaseNode* node);
    void erase(const_iterator it);
  };

  List<NodeType, Alloc> list_;
  std::vector<typename List<NodeType, Alloc>::BaseNode*> array_;
  float max_load_factor_ = 0.5;

  void clear();

 public:
  using iterator = typename List<NodeType, Alloc>::iterator;
  using const_iterator = typename List<NodeType, Alloc>::const_iterator;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;

  UnorderedMap() = default;
  UnorderedMap(const UnorderedMap& other);
  explicit UnorderedMap(UnorderedMap&& other);

  ~UnorderedMap();

  UnorderedMap& operator=(const UnorderedMap& other);
  UnorderedMap& operator=(UnorderedMap&& other);

  Value& operator[](const Key& key);
  Value& operator[](Key&& key);
  Value& at(const Key& key);
  const Value& at(const Key& key) const;
  iterator find(const Key& key);
  const_iterator find(const Key& key) const;
  void swap(UnorderedMap& other);

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args);

  std::pair<iterator, bool> insert(const NodeType& value);
  std::pair<iterator, bool> insert(NodeType&& value);

  template <typename InputIt>
  void insert(InputIt first, InputIt last);

  iterator erase(iterator pos);
  void erase(iterator first, iterator last);

  float load_factor() const;
  float max_load_factor() const;
  void max_load_factor(float new_factor);
  void reserve(size_t count);

  void rehash(size_t count);
  size_t size() const;
};

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename... Args>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::emplace(
    Node* node, Args&&... args) {
  try {
    typename std::allocator_traits<NodeAlloc>::template rebind_alloc<NodeType>
        node_type_alloc_(node_alloc_);
    std::allocator_traits<
        typename std::allocator_traits<NodeAlloc>::template rebind_alloc<
            NodeType>>::construct(node_type_alloc_, &(node->value),
                                  std::forward<Args>(args)...);
    node->hash = Hash{}(node->value.first);
  } catch (...) {
    throw "OLOLO";
  }
}
template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, typename Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::BaseNode*
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::getNode() {
  return cur_node_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<
    T, Allocator>::BaseNode::BaseNode(List::BaseNode* prev,
                                      List::BaseNode* next)
    : prev(prev), next(next) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::BaseNode::
BaseNode(UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::List<T, Allocator>::BaseNode&& other)
    : prev(std::move(other.prev)),
      next(std::move(other.next)),
      hash(std::move(other.hash)) {
  other.prev = &other;
  other.next = &other;
  other.hash = 0;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::BaseNode&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::BaseNode::
operator=(UnorderedMap<Key, Value, Hash, Equal,
                       Alloc>::List<T, Allocator>::BaseNode&& other) {
  if (other.prev == &other) {
    prev = this;
    next = this;
  } else {
    prev = other.prev;
    next = other.next;
  }
  prev->next = this;
  next->prev = this;

  hash = other.hash;

  other.prev = &other;
  other.next = &other;
  other.hash = 0;

  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::Node::Node(
    List<T, Allocator>::BaseNode* prev, List<T, Allocator>::BaseNode* next)
    : BaseNode(prev, next), value() {
  try {
    BaseNode::hash = Hash{}(value.first);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::Node::Node(
    List<T, Allocator>::BaseNode* prev, List<T, Allocator>::BaseNode* next,
    const T& value)
    : BaseNode(prev, next), value(value) {
  try {
    BaseNode::hash = Hash{}(value->first);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List()
    : end_(BaseNode{&end_, &end_}), size_(0), node_alloc_(Allocator()) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::clear() {
  while (end_.prev != &end_) {
    pop_back();
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <bool has_default_constructor>
void UnorderedMap<Key, Value, Hash, Equal,
                  Alloc>::List<T, Allocator>::construct(bool from_int,
                                                        size_t size,
                                                        const T* elem,
                                                        const List* other) {
  if (size == 0) {
    return;
  }
  try {
    if (from_int) {
      if constexpr (has_default_constructor) {
        for (size_t i = 0; i < size; ++i) {
          Node* new_node =
              std::allocator_traits<NodeAlloc>::allocate(node_alloc_, 1);
          std::allocator_traits<NodeAlloc>::construct(node_alloc_, new_node,
                                                      end_.prev, &end_);
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
    throw;
  }
  size_ = size;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    size_t size)
    : List() {
  construct<true>(true, size, nullptr, nullptr);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    size_t size, const T& elem)
    : List() {
  construct<false>(true, size, &elem, nullptr);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    const Allocator& alloc)
    : end_(BaseNode{&end_, &end_}), size_(0), node_alloc_(alloc) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    size_t size, const Allocator& alloc)
    : List(alloc) {
  construct<true>(true, size, nullptr, nullptr);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    size_t size, const T& elem, const Allocator& alloc)
    : List(alloc) {
  construct<false>(true, size, &elem, nullptr);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::NodeAlloc&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T,
                                                   Allocator>::get_allocator() {
  return node_alloc_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    const List<T, Allocator>& other)
    : end_(BaseNode{&end_, &end_}), size_(0) {
  node_alloc_ =
      std::allocator_traits<Allocator>::select_on_container_copy_construction(
          other.node_alloc_);
  construct<false>(false, other.size_, nullptr, &other);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::List(
    List<T, Allocator>&& other)
    : end_(std::move(other.end_)),
      size_(std::move(other.size_)),
      node_alloc_(std::move(other.node_alloc_)) {
  other.size_ = 0;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<T,
                                                                     Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::operator=(
    const List<T, Allocator>& other) {
  clear();
  if (std::allocator_traits<
      Allocator>::propagate_on_container_copy_assignment::value) {
    node_alloc_ = other.node_alloc_;
  }
  construct<false>(false, other.size_, nullptr, &other);
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<T,
                                                                     Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::operator=(
    List<T, Allocator>&& other) {
  clear();
  if (std::allocator_traits<
      Allocator>::propagate_on_container_move_assignment::value) {
    node_alloc_ = std::move(other.node_alloc_);
  }
  end_ = std::move(other.end_);
  size_ = other.size_;
  other.size_ = 0;
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::size()
const {
  return size_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal,
                  Alloc>::List<T, Allocator>::push_back(const T& elem) {
  insert(iterator(&end_), elem);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T,
                                                        Allocator>::pop_back() {
  erase(--iterator(&end_));
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal,
                  Alloc>::List<T, Allocator>::push_front(const T& elem) {
  insert(iterator(end_.next), elem);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal,
                  Alloc>::List<T, Allocator>::pop_front() {
  erase(iterator(end_.next));
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::~List() {
  clear();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::iterator_()
    : cur_node_(nullptr) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::iterator_<
    U>::iterator_(const List<T, Allocator>::iterator_<U>& other)
    : cur_node_(other.cur_node_) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::iterator_(BaseNode* node)
    : cur_node_(node) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::template iterator_<U>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::iterator_<
    U>::operator=(const iterator_<U>& other) {
  cur_node_ = other.cur_node_;
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::template iterator_<U>&
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::operator++() {
  cur_node_ = cur_node_->next;
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::template iterator_<U>
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::operator++(int) {
  iterator to_return = iterator(cur_node_);
  cur_node_ = cur_node_->next;
  return to_return;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::template iterator_<U>&
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::operator--() {
  cur_node_ = cur_node_->prev;
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::template iterator_<U>
UnorderedMap<Key, Value, Hash, Equal,
             Alloc>::List<T, Allocator>::iterator_<U>::operator--(int) {
  iterator to_return = iterator(cur_node_);
  cur_node_ = cur_node_->prev;
  return to_return;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
U& UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::iterator_<U>::operator*() const {
  return static_cast<Node*>(cur_node_)->value;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
U* UnorderedMap<Key, Value, Hash, Equal,
                Alloc>::List<T, Allocator>::iterator_<U>::operator->() const {
  return &static_cast<Node*>(cur_node_)->value;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<
    T, Allocator>::iterator_<U>::operator==(const iterator_<U>& other) const {
  return cur_node_ == other.cur_node_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<
    T, Allocator>::iterator_<U>::operator!=(const iterator_<U>& other) const {
  return !(*this == other);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::begin() {
  return iterator(end_.next);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::begin()
const {
  return const_iterator(end_.next);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::cbegin()
const {
  return const_iterator(end_.next);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::end() {
  return iterator(&end_);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::end() const {
  return const_iterator(end_.next->prev);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::cend() const {
  return const_iterator(end_.next->prev);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::rbegin() {
  return std::reverse_iterator(end());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal,
                      Alloc>::template List<T, Allocator>::reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::rend() {
  return std::reverse_iterator(begin());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::const_reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::rbegin()
const {
  return std::reverse_iterator(end());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::const_reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::rend() const {
  return std::reverse_iterator(begin());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::const_reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::crbegin()
const {
  return std::reverse_iterator(List<T, Allocator>::const_iterator(end_.next));
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template List<
    T, Allocator>::const_reverse_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::crend()
const {
  return std::reverse_iterator(List<T, Allocator>::const_iterator(&end_));
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
template <typename U>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::iterator_<
    U>::operator iterator_<const U>() const {
  List<T, Allocator>::iterator_<const U> to_return;
  to_return.cur_node_ = cur_node_;
  return to_return;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::insert(
    List<T, Allocator>::const_iterator it, const T& elem) {
  Node* new_node;
  BaseNode* prev = it.cur_node_->prev;
  try {
    new_node = std::allocator_traits<NodeAlloc>::allocate(node_alloc_, 1);
    std::allocator_traits<NodeAlloc>::construct(node_alloc_, new_node, prev,
                                                it.cur_node_, elem);
    prev->next = static_cast<BaseNode*>(new_node);
    it.cur_node_->prev = static_cast<BaseNode*>(new_node);
  } catch (...) {
    if (new_node == nullptr) {
      return;
    }
    it.cur_node_->prev = prev;
    prev->next = it.cur_node_;
    throw;
  }
  ++size_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal,
                  Alloc>::List<T, Allocator>::insertNode(const_iterator it,
BaseNode* node) {
node->prev->next = node->next;
node->next->prev = node->prev;
node->prev = it.getNode()->prev;
node->next = it.getNode();
it.getNode()->prev->next = node;
it.getNode()->prev = node;
++size_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename T, class Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::List<T, Allocator>::erase(
    List<T, Allocator>::const_iterator it) {
  try {
    it.cur_node_->prev->next = it.cur_node_->next;
    it.cur_node_->next->prev = it.cur_node_->prev;
    std::allocator_traits<NodeAlloc>::deallocate(
        node_alloc_, static_cast<Node*>(it.cur_node_), 1);
  } catch (...) {
    throw;
  }
  --size_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::clear() {
  list_.clear();
  array_.assign(array_.size(), nullptr);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::~UnorderedMap() {
  clear();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() {
  return list_.begin();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() const {
  return const_iterator(list_.cbegin());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::cbegin() const {
  return list_.cbegin();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() {
  return list_.end();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() const {
  return list_.end();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::cend() const {
  return list_.cend();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&&... args) {
  typename List<NodeType, Alloc>::Node* new_node = std::allocator_traits<
      typename List<NodeType, Alloc>::NodeAlloc>::allocate(list_.node_alloc_,
                                                           1);
  try {
    typename std::allocator_traits<typename List<NodeType, Alloc>::NodeAlloc>::
    template rebind_alloc<NodeType>
        node_type_alloc_(list_.node_alloc_);
    std::allocator_traits<typename std::allocator_traits<typename List<
        NodeType, Alloc>::NodeAlloc>::template rebind_alloc<NodeType>>::
    construct(node_type_alloc_, &(new_node->value),
              std::forward<Args>(args)...);
    new_node->hash = Hash{}(new_node->value.first);
  } catch (...) {
    throw "OLOLO";
  }
  auto it = find(new_node->value.first);
  if (it != end()) {
    std::allocator_traits<typename List<NodeType, Alloc>::NodeAlloc>::destroy(
        list_.node_alloc_, new_node);
    std::allocator_traits<typename List<NodeType, Alloc>::NodeAlloc>::
    deallocate(list_.node_alloc_, new_node, 1);
    return std::make_pair(it, false);
  }
  if (max_load_factor_ * array_.size() <= list_.size()) {
    rehash(array_.size() > 0 ? array_.size() * 2 : 1);
  }
  size_t index = Hash{}(new_node->value.first) % array_.size();
  typename List<NodeType, Alloc>::BaseNode* place_to_insert;
  if (array_[index] == nullptr) {
    place_to_insert = &list_.end_;
    array_[index] = new_node;
  } else {
    iterator iter = array_[index];
    while (iter != list_.end() &&
        iter.getNode()->hash % array_.size() == index) {
      ++iter;
    }
    place_to_insert = iter.getNode();
  }
  new_node->prev = place_to_insert->prev;
  new_node->next = place_to_insert;
  place_to_insert->prev->next = new_node;
  place_to_insert->prev = new_node;
  ++list_.size_;
  return std::make_pair(--iterator(place_to_insert), true);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(size_t count) {
  List<NodeType, Alloc> new_list;
  array_.resize(count);
  array_.assign(count, nullptr);
  iterator next_it;
  for (iterator it = begin(); it != end(); it = next_it) {
    next_it = it;
    ++next_it;
    size_t index = it.getNode()->hash % array_.size();
    if (array_[index] == nullptr) {
      new_list.insertNode(new_list.end(), it.getNode());
      array_[index] = it.getNode();
    } else {
      iterator iter = array_[index];
      while (iter != new_list.end() &&
          iter.getNode()->hash % array_.size() == index) {
        ++iter;
      }
      new_list.insertNode(iter, it.getNode());
    }
  }
  list_ = std::move(new_list);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(
    const UnorderedMap::NodeType& value) {
  return emplace(const_cast<Key&>(value.first), value.second);
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(
    UnorderedMap::NodeType&& value) {
  return emplace(std::move(const_cast<Key&>(value.first)),
                 std::move(value.second));
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
template <typename InputIt>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIt first,
                                                          InputIt last) {
  for (auto it = first; it != last; ++it) {
    insert(*it);
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
size_t UnorderedMap<Key, Value, Hash, Equal, Alloc>::size() const {
  return list_.size();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(
    UnorderedMap::iterator pos) {
  try {
    size_t index = Hash{}(pos->first) % array_.size();
    auto to_return = pos;
    ++to_return;
    if (array_[index] == pos.getNode()) {
      if (pos.getNode()->next->hash % array_.size() == index) {
        array_[index] = pos.getNode()->next;
      } else {
        array_[index] = nullptr;
      }
    }
    list_.erase(pos);
    return to_return;
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(iterator first,
iterator last) {
for (iterator it = first; it != last;) {
it = erase(it);
}
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::load_factor() const {
  return static_cast<float>(list_.size()) / array_.size();
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor() const {
  return max_load_factor_;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor(
    float new_factor) {
  max_load_factor_ = new_factor;
  reserve(size());
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t count) {
  if (array_.size() * max_load_factor_ < count) {
    rehash(count / max_load_factor_ + 1);
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(
    const UnorderedMap<Key, Value, Hash, Equal, Alloc>& other)
    : max_load_factor_(other.max_load_factor_) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    insert(*it);
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(
    UnorderedMap<Key, Value, Hash, Equal, Alloc>&& other)
    : list_(std::move(other.list_)),
      array_(std::move(other.array_)),
      max_load_factor_(std::move(other.max_load_factor_)) {}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(
    const UnorderedMap<Key, Value, Hash, Equal, Alloc>& other) {
  if (this == &other) {
    return *this;
  }

  max_load_factor_ = other.max_load_factor_;

  try {
    clear();
    reserve(other.size());
    for (auto it = other.begin(); it != other.end(); ++it) {
      (*this)[it->first] = it->second;
    }
  } catch (...) {
    clear();
    throw;
  }
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(
    UnorderedMap<Key, Value, Hash, Equal, Alloc>&& other) {
  max_load_factor_ = std::move(other.max_load_factor_);
  list_ = std::move(other.list_);
  array_ = std::move(other.array_);
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](
    const Key& key) {
  auto to_return = emplace(key, std::move(Value()));
  return to_return.first->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](Key&& key) {
  auto to_return = emplace(std::move(key), std::move(Value()));
  return to_return.first->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
  iterator it = find(key);
  if (it == end()) {
    throw std::out_of_range("LOL");
  }
  return it->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
const Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(
    const Key& key) const {
  const_iterator it = find(key);
  if (it == end()) {
    throw std::out_of_range("LOL");
  }
  return it->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) {
  if (size() == 0) {
    return end();
  }
  try {
    size_t index = Hash{}(key) % array_.size();
    if (array_[index] == nullptr) {
      return end();
    }
    for (typename List<NodeType, Alloc>::iterator it = array_[index];
         it != list_.end() && it.getNode()->hash % array_.size() == index;
         ++it) {
      if (Equal{}(it->first, key)) {
        return it;
      }
    }
    return end();
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
    typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) const {
  if (size() == 0) {
    return end();
  }
  try {
    size_t index = Hash{}(key) % array_.size();
    if (array_[index] == nullptr) {
      return end();
    }
    for (typename List<NodeType, Alloc>::const_iterator it = array_[index];
         it != list_.end() && it.getNode()->hash % array_.size() == index;
         ++it) {
      if (Equal{}(it->first, key)) {
        return it;
      }
    }
    return end();
  } catch (...) {
    throw;
  }
}

