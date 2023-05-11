#include <stdexcept>

template <typename T>
class Deque {
 public:
  Deque() = default;
  Deque(const Deque& other);
  Deque(int size, const T& element);
  explicit Deque(int size);
  ~Deque();
  Deque& operator=(const Deque<T>& other);
  size_t size() const;
  T& operator[](size_t index);
  const T& operator[](size_t index) const;
  T& at(size_t index);
  const T& at(size_t index) const;
  void push_back(const T& elem);
  void pop_back();
  void push_front(const T& elem);
  void pop_front();
  template <typename U>
  class iterator_ {
   public:
    friend Deque;
    using value_type = U;
    using pointer = value_type*;
    using difference_type = int;
    using reference = value_type&;
    using iterator_category = std::random_access_iterator_tag;
    iterator_();
    iterator_(const iterator_<value_type>& other);
    iterator_(pointer* row, size_t index);
    ~iterator_() = default;
    iterator_& operator=(const iterator_<value_type>& other);
    operator iterator_<const value_type>();
    iterator_<value_type>& operator++();
    iterator_<value_type> operator++(int);
    iterator_<value_type>& operator--();
    iterator_<value_type> operator--(
        int);  // объясни пж че здесь надо, не константный же итератор мне
               // возвращать
    iterator_<value_type>& operator+=(int delta);
    iterator_<value_type> operator+(int delta) const;
    iterator_<value_type>& operator-=(int delta);
    iterator_<value_type> operator-(int delta) const;
    int operator-(const iterator_<value_type>& other);
    reference operator*() const;
    pointer operator->() const;
    bool operator<(const iterator_<U>& other) const;
    bool operator>(const iterator_<U>& other) const;
    bool operator<=(const iterator_<U>& other) const;
    bool operator>=(const iterator_<U>& other) const;
    bool operator==(const iterator_<U>& other) const;
    bool operator!=(const iterator_<U>& other) const;

   private:
    T** row_ = nullptr;
    size_t index_ = 0;
    pointer element_ = nullptr;
  };
  using iterator = iterator_<T>;
  using const_iterator = iterator_<const T>;
  using riterator = std::reverse_iterator<iterator>;
  using criterator = std::reverse_iterator<const_iterator>;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  riterator rbegin();
  riterator rend();
  criterator rbegin() const;
  criterator rend() const;
  criterator crbegin() const;
  criterator crend() const;
  void insert(iterator place, const T& elem);
  void erase(iterator place);

 private:
  template <bool has_default_const>
  void construct(bool from_int, const Deque* other, const T* elem, size_t size);
  void swap(Deque& other);
  static const int row_size_ = 10;
  T** array_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;
  iterator begin_;
  iterator end_;
  int real_first_row = -1;
  int real_last_row = -1;
};

template <typename T>
template <bool has_default_const>
void Deque<T>::construct(bool from_int, const Deque<T>* other, const T* elem,
                         size_t size) {
  if (size == 0) {
    return;
  }
  size_t i = 0;
  try {
    for (; i < size; ++i) {
      size_t row_index_ = from_int ? i / row_size_
                                   : other->begin_.row_ - other->array_ +
                                         (i + other->begin_.index_) / row_size_;
      size_t elem_index_ =
          (i + (from_int ? 0 : other->begin_.index_)) % row_size_;
      if (i == 0 || elem_index_ == 0) {
        *(array_ + row_index_) =
            reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
      }
      if (from_int) {
        if constexpr (has_default_const) {
          new (*(array_ + row_index_) + elem_index_) T();
        } else {
          new (*(array_ + row_index_) + elem_index_) T(*elem);
        }
      } else {
        new (*(array_ + row_index_) + elem_index_) T(*(other->begin_ + i));
      }
    }
    if (size % row_size_ == 0) {
      *(array_ + size / row_size_) =
          reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
    }
    begin_ = from_int ? iterator(array_, 0)
                      : iterator(array_ + (other->begin_.row_ - other->array_),
                                 other->begin().index_);
    end_ = from_int ? iterator(array_ + size / row_size_, size % row_size_)
                    : iterator(array_ + (other->end_.row_ - other->array_),
                               other->end().index_);
    real_first_row = (from_int ? 0 : other->real_first_row);
    real_last_row = from_int ? size / row_size_ : other->real_last_row;
  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      size_t row_index_ = from_int ? j / row_size_
                                   : other->begin_.row_ - other->array_ +
                                         (j + other->begin_.index_) / row_size_;
      size_t elem_index_ =
          (j + (from_int ? 0 : other->begin_.index_)) % row_size_;
      (*(array_ + row_index_) + elem_index_)->~T();
    }
    for (size_t j = 0; j < i; ++j) {
      size_t row_index_ = from_int ? j / row_size_
                                   : other->begin_.row_ - other->array_ +
                                         (j + other->begin_.index_) / row_size_;
      size_t elem_index_ =
          (j + (from_int ? 0 : other->begin_.index_)) % row_size_;
      if (j == 0 || elem_index_ == 0) {
        delete[] reinterpret_cast<char*>(*(array_ + row_index_));
      }
    }
    delete[] array_;
    throw;
  }
}

template <typename T>
Deque<T>::Deque(const Deque<T>& other)
    : array_(reinterpret_cast<T**>(new char[sizeof(T*) * (other.capacity_)])),
      size_(other.size_),
      capacity_(other.capacity_) {
  construct<false>(false, &other, nullptr, other.size_);
}

template <typename T>
Deque<T>::Deque(int size, const T& element)
    : array_(reinterpret_cast<T**>(
          new char[sizeof(T*) * (size + row_size_) / row_size_])),
      size_(size),
      capacity_((size + row_size_) / row_size_) {
  construct<false>(true, nullptr, &element, size);
}

template <typename T>
Deque<T>::Deque(int size)
    : array_(reinterpret_cast<T**>(
          new char[sizeof(T*) * (size + row_size_) / row_size_])),
      size_(size),
      capacity_((size + row_size_) / row_size_) {
  construct<true>(true, nullptr, nullptr, size);
}

template <typename T>
void Deque<T>::swap(Deque<T>& other) {
  std::swap(array_, other.array_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(real_first_row, other.real_first_row);
  std::swap(real_last_row, other.real_last_row);
  iterator temp = other.begin();
  other.begin_ = begin_;
  begin_ = temp;
  temp = other.end();
  other.end_ = end_;
  end_ = temp;
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  if (this == &other) {
    return *this;
  }
  swap(other);
  return *this;
}

template <typename T>
size_t Deque<T>::size() const {
  return size_;
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
  return *(begin_ + index);
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  return *(begin_ + index);
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("");
  }
  return *(begin_ + index);
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index > size_) {
    throw std::out_of_range("");
  }
  return *(begin_ + index);
}

template <typename T>
void Deque<T>::push_back(const T& elem) {
  if (array_ == nullptr) {
    array_ = reinterpret_cast<T**>(new char[sizeof(T*)]);
    capacity_ = 1;
    size_ = 1;
    construct<false>(true, nullptr, &elem, 1);
    return;
  }
  if (end_.row_ == array_ + capacity_ - 1 && end_.index_ == row_size_ - 1) {
    T** new_arr = reinterpret_cast<T**>(new char[capacity_ * 3 * sizeof(T*)]);
    iterator i = begin_;
    try {
      for (; i < end_;) {
        if (i.index_ == row_size_ - 1) {
          new_arr[capacity_ + (i.row_ - array_)] = *i.row_;
          if (row_size_ < end_ - i) {
            i += row_size_;
          } else {
            ++i;
          }
        } else {
          ++i;
        }
      }
      new_arr[capacity_ + (i.row_ - array_)] = *i.row_;
      *(new_arr + capacity_ + (end_.row_ - array_) + 1) =
          reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
      new (*(new_arr + capacity_ + (end_.row_ - array_)) + row_size_ - 1)
          T(elem);
      real_last_row = capacity_ + (end_.row_ - array_) + 1;
      real_first_row += capacity_;
      begin_ =
          iterator(new_arr + capacity_ + (begin_.row_ - array_), begin_.index_);
      end_ = iterator(new_arr + capacity_ + (end_.row_ - array_) + 1, 0);
      capacity_ *= 3;
    } catch (...) {
      for (iterator j = begin_; j != i; ++j) {
        (*(new_arr + capacity_ + (j.row_ - array_)) + j.index_)->~T();
      }
      for (size_t j = capacity_; j <= capacity_ + (end_.row_ - array_) + 1;
           ++j) {
        delete[] reinterpret_cast<char*>(new_arr[j]);
      }
      delete[] reinterpret_cast<char*>(new_arr);
      throw;
    }
    for (iterator i = begin_; i != end_; ++i) {
      (i.element_)->~T();
    }
    delete[] reinterpret_cast<char*>(array_);
    array_ = new_arr;
  } else {
    new (end_.element_) T(elem);
    if (end_.index_ == row_size_ - 1 &&
        end_.row_ - array_ + 1 > real_last_row) {
      *(end_.row_ + 1) = reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
      real_last_row = end_.row_ - array_ + 1;
    }
    ++end_;
  }
  ++size_;
}

template <typename T>
void Deque<T>::pop_back() {
  --size_;
  ((--end_).element_)->~T();
}

template <typename T>
void Deque<T>::push_front(const T& elem) {
  if (array_ == nullptr) {
    array_ = reinterpret_cast<T**>(new char[sizeof(T*)]);
    capacity_ = 1;
    size_ = 1;
    construct<false>(true, nullptr, &elem, 1);
    return;
  }
  if (begin_.row_ == array_ && begin_.index_ == 0) {
    T** new_arr = reinterpret_cast<T**>(new char[capacity_ * 3 * sizeof(T*)]);
    iterator i = begin_;
    try {
      for (; i < end_;) {
        if (i.index_ == row_size_ - 1) {
          new_arr[capacity_ + (i.row_ - array_)] = *i.row_;
          if (end_ - i > row_size_) {
            i += row_size_;
          } else {
            ++i;
          }
        } else {
          ++i;
        }
      }
      new_arr[capacity_ + (end_.row_ - array_)] = *end_.row_;
      *(new_arr + capacity_ - 1) =
          reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
      new (*(new_arr + capacity_ - 1) + row_size_ - 1) T(elem);
      real_first_row = capacity_ - 1;
      real_last_row += capacity_;
      begin_ = iterator(new_arr + capacity_ - 1, row_size_ - 1);
      end_ = iterator(new_arr + capacity_ + (end_.row_ - array_), end_.index_);
      delete[] reinterpret_cast<char*>(array_);
      array_ = new_arr;
      capacity_ *= 3;
    } catch (...) {
      for (iterator j = begin_; j != i; ++j) {
        (*(new_arr + capacity_ + (j.row_ - array_)) + j.index_)->~T();
      }
      for (size_t j = capacity_ - 1; j <= capacity_ + (end_.row_ - array_);
           ++j) {
        delete[] reinterpret_cast<char*>(new_arr[j]);
      }
      delete[] reinterpret_cast<char*>(new_arr);
      throw;
    }
  } else {
    if (begin_.index_ == 0 && begin_.row_ - array_ - 1 < real_first_row) {
      *(begin_.row_ - 1) =
          reinterpret_cast<T*>(new char[row_size_ * sizeof(T)]);
      real_first_row = begin_.row_ - array_ - 1;
    }
    --begin_;
    new (begin_.element_) T(elem);
  }
  ++size_;
}

template <typename T>
void Deque<T>::pop_front() {
  --size_;
  ((begin_++).element_)->~T();
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>& Deque<T>::iterator_<U>::operator++() {
  if (row_ == nullptr) {
    return *this;
  }
  if (index_ == row_size_ - 1) {
    index_ = 0;
    ++row_;
    element_ = *row_ + index_;
  } else {
    ++index_;
    ++element_;
  }
  return *this;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U> Deque<T>::iterator_<U>::operator++(
    int) {
  Deque<T>::iterator_<U> copy = *this;
  ++*this;
  return copy;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>& Deque<T>::iterator_<U>::operator--() {
  if (row_ == nullptr) {
    return *this;
  }
  if (index_ == 0) {
    index_ = row_size_ - 1;
    --row_;
    element_ = *row_ + index_;
  } else {
    --index_;
    --element_;
  }
  return *this;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U> Deque<T>::iterator_<U>::operator--(
    int) {
  Deque<T>::iterator_<U> copy = *this;
  --(*this);
  return *this;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>&
    Deque<T>::template iterator_<U>::operator+=(int delta) {
  if (row_ == nullptr) {
    return *this;
  }
  if (delta >= 0) {
    row_ += (index_ + delta) / row_size_;
    index_ = (index_ + delta) % row_size_;
  } else {
    delta = -delta;
    row_ -= (row_size_ - 1 - index_ + delta) / row_size_;
    index_ = index_ >= static_cast<size_t>(delta)
                 ? index_ - delta
                 : (row_size_ + (index_ - delta) % row_size_) % row_size_;
  }
  element_ = *row_ + index_;
  return *this;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>
    Deque<T>::template iterator_<U>::operator+(int delta) const {
  iterator_<U> to_return = *this;
  to_return += delta;
  return to_return;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>&
    Deque<T>::template iterator_<U>::operator-=(int delta) {
  *this += (-delta);
  return *this;
}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>
    Deque<T>::template iterator_<U>::operator-(int delta) const {
  iterator_<U> to_return = *this;
  to_return -= delta;
  return to_return;
}

template <typename T>
template <typename U>
int Deque<T>::iterator_<U>::operator-(const iterator_<value_type>& other) {
  return (row_ - other.row_) * row_size_ + (index_ - other.index_);
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator<(
    const typename Deque<T>::iterator_<U>& other) const {
  return row_ == other.row_ ? index_ < other.index_ : row_ < other.row_;
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator>(
    const typename Deque<T>::iterator_<U>& other) const {
  return other < *this;
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator<=(
    const typename Deque<T>::iterator_<U>& other) const {
  return !(other < *this);
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator>=(
    const typename Deque<T>::iterator_<U>& other) const {
  return !(*this < other);
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator==(
    const typename Deque<T>::iterator_<U>& other) const {
  return row_ == other.row_ && index_ == other.index_;
}

template <typename T>
template <typename U>
bool Deque<T>::iterator_<U>::operator!=(
    const typename Deque<T>::iterator_<U>& other) const {
  return !(*this == other);
}

template <typename T>
template <typename U>
U& Deque<T>::iterator_<U>::operator*() const {
  return *element_;
}

template <typename T>
template <typename U>
U* Deque<T>::iterator_<U>::operator->() const {
  return element_;
}

template <typename T>
template <typename U>
Deque<T>::iterator_<U>::iterator_(U** row, size_t index)
    : row_(row), index_(index), element_(*row + index) {}

template <typename T>
template <typename U>
Deque<T>::iterator_<U>::iterator_()
    : row_(nullptr), index_(0), element_(nullptr) {}

template <typename T>
template <typename U>
Deque<T>::iterator_<U>::iterator_(const iterator_<U>& other)
    : row_(other.row_), index_(other.index_), element_(other.element_) {}

template <typename T>
template <typename U>
typename Deque<T>::template iterator_<U>& Deque<T>::iterator_<U>::operator=(
    const iterator_<U>& other) {
  if (this == &other) {
    return *this;
  }
  row_ = other.row_;
  index_ = other.index_;
  element_ = other.element_;
  return *this;
}

template <typename T>
template <typename U>
Deque<T>::iterator_<U>::operator iterator_<const U>() {
  Deque<T>::iterator_<const U> to_return;
  to_return.row_ = row_;
  to_return.index_ = index_;
  to_return.element_ = element_;
  return to_return;
}

template <typename T>
typename Deque<T>::iterator Deque<T>::begin() {
  return begin_;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
  const_iterator to_return;
  to_return.row_ = begin_.row_;
  to_return.element_ = begin_.element_;
  to_return.index_ = begin_.index_;
  return to_return;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  const_iterator to_return;
  to_return.row_ = begin_.row_;
  to_return.element_ = begin_.element_;
  to_return.index_ = begin_.index_;
  return to_return;
}

template <typename T>
typename Deque<T>::iterator Deque<T>::end() {
  return end_;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::end() const {
  const_iterator to_return;
  to_return.row_ = end_.row_;
  to_return.element_ = end_.element_;
  to_return.index_ = end_.index_;
  return to_return;
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
  const_iterator to_return;
  to_return.row_ = end_.row_;
  to_return.element_ = end_.element_;
  to_return.index_ = end_.index_;
  return to_return;
}

template <typename T>
typename Deque<T>::riterator Deque<T>::rbegin() {
  return std::reverse_iterator(end_);
}

template <typename T>
typename Deque<T>::criterator Deque<T>::rbegin() const {
  return std::reverse_iterator(end_);
}

template <typename T>
typename Deque<T>::riterator Deque<T>::rend() {
  return std::reverse_iterator(begin_);
}

template <typename T>
typename Deque<T>::criterator Deque<T>::rend() const {
  return std::reverse_iterator(begin_);
}

template <typename T>
typename Deque<T>::criterator Deque<T>::crbegin() const {
  return std::reverse_iterator(Deque<T>::const_iterator(end_));
}

template <typename T>
typename Deque<T>::criterator Deque<T>::crend() const {
  return std::reverse_iterator(Deque<T>::const_iterator(begin_));
}

template <typename T>
Deque<T>::~Deque() {
  if (array_ == nullptr) {
    return;
  }
  if (begin_.row_ != nullptr) {
    for (iterator i = begin_; i < end_; ++i) {
      (i.element_)->~T();
    }
    if (real_first_row != -1) {
      for (int i = real_first_row; i <= real_last_row; ++i) {
        delete[] reinterpret_cast<char*>(array_[i]);
      }
    }
    delete[] array_;
  }
}

template <typename T>
void Deque<T>::insert(iterator place, const T& elem) {
  if (begin_.row_ == nullptr) {
    push_back(elem);
    return;
  }
  if (end_.index_ == row_size_ - 1) {
    size_t remember_row = place.row_ - array_;
    size_t remember_index = place.index_;
    size_t remember_cap = capacity_;
    push_back(*(end_ - 1).element_);
    place = iterator(array_ + remember_row +
                         ((remember_cap == capacity_) ? 0 : remember_cap),
                     remember_index);
  } else {
    ++end_;
    ++size_;
  }
  for (iterator i = end_ - 1; i > place; --i) {
    *i.element_ = *(i - 1).element_;
  }
  *place.element_ = elem;
}

template <typename T>
void Deque<T>::erase(iterator place) {
  for (iterator i = place + 1; i < end_; ++i) {
    *(i - 1).element_ = *i.element_;
  }
  pop_back();
}
