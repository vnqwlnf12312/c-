#include <iostream>
#include <cstring>

class String {
 private:
  void enlarge_capacity_();
  bool is_equal(const String&, size_t) const;
  void swap(String& str);
 public:
  size_t length() const;
  size_t size() const;
  size_t capacity() const;
  void push_back(const char);
  void pop_back();
  char& front();
  const char& front() const;
  char& back();
  const char& back() const;
  size_t find(const String&) const;
  size_t rfind(const String& substring) const;
  String substr(size_t start, size_t count) const;
  bool empty() const;
  void clear();
  void shrink_to_fit();
  char* data();
  const char* data() const;

  size_t size_ = 0;
  size_t capacity_ = 1;
  char* array_;

  char& operator[](size_t index) {
    return array_[index];
  }
  
  const char& operator[](size_t index) const {
    return array_[index];
  }

  String& operator=(String str) {
    swap(str);
    return *this;
  }

  String& operator+=(const String& str) {
    if (capacity_ < size_ + str.size_) {
      char* to_delete = array_;
      array_ = new char[size_ + str.size_ + 1];
      capacity_ = size_ + str.size_;
      memcpy(array_, to_delete, size_);
      delete to_delete;
    }
    memcpy(array_ + size_, str.array_, str.size_ + 1);
    size_ += str.size_;
    return *this;
  }

  String& operator+=(const char a) {
    push_back(a);
    return *this;
  }

  String(size_t n) : size_(0), capacity_(n), array_(new char[n + 1]) {
    array_[0] = '\0';
  }

  String(const char* str)
      : size_(strlen(str)), capacity_(size_), array_(new char[size_ + 1]) {
    memcpy(array_, str, size_ + 1);
  }

  String(const size_t n, const char c)
      : size_(n), capacity_(n), array_(new char[n + 1]) {
    memset(array_, c, n);
    array_[n] = '\0';
  }

  String(const String& str)
      : size_(str.size_),
        capacity_(str.capacity_),
        array_(new char[str.capacity_ + 1]) {
    memcpy(array_, str.array_, size_ + 1);
  }

  String() : size_(0), capacity_(0), array_(new char[1]) {
    array_[0] = '\0';
  }

  ~String() {
    delete[] array_;
  }
};

void String::enlarge_capacity_() {
  capacity_ *= 2;
  char* to_delete = array_;
  array_ = new char[capacity_ + 2];
  memcpy(array_, to_delete, size_ + 1);
  delete to_delete;
}

bool String::is_equal(const String& substring, size_t i) const {
  for (size_t j = 0; j < substring.size_; ++j) {
    if (array_[i + j] != substring.array_[j]) {
      return false;
    }
  }
  return true;
}

void String::swap(String& str) {
  std::swap(array_, str.array_);
  std::swap(size_, str.size_);
  std::swap(capacity_, str.capacity_);
}

size_t String::length() const {
  return size_;
}

size_t String::size() const {
  return size_;
}

size_t String::capacity() const {
  return capacity_;
}

void String::push_back(const char a) {
  if (size_ >= capacity_) {
    enlarge_capacity_();
  }
  array_[size_] = a;
  array_[size_ + 1] = '\0';
  ++size_;
}

void String::pop_back() {
  --size_;
}

char& String::front() {
  return array_[0];
}

const char& String::front() const {
  return array_[0];
}

char& String::back() {
  return array_[size_ - 1];
}

const char& String::back() const {
  return array_[size_ - 1];
}

size_t String::find(const String& substring) const {
  size_t end_of_search = size_ - substring.size_;
  for (size_t i = 0; i < end_of_search; ++i) {
    if (is_equal(substring, i)) {
      return i;
    }
  }
  return length();
}

size_t String::rfind(const String& substring) const {
  for (size_t i = size_ - substring.size_ + 1; i >= 1; --i) {
    if (is_equal(substring, i - 1)) {
      return i - 1;
    }
  }
  return length();
}

String String::substr(size_t start, size_t count) const {
  String to_return(count);
  memcpy(to_return.array_, array_ + start, size_);
  to_return[count] = '\0';
  to_return.size_ = count;
  to_return.capacity_ = count;
  return to_return;
}

bool String::empty() const {
  return size_ == 0;
}

void String::clear() {
  size_ = 0;
}

void String::shrink_to_fit() {
  if (size_ < capacity_) {
    char* to_delete = array_;
    array_ = new char[size_ + 1];
    memcpy(array_, to_delete, size_ + 1);
    delete to_delete;
    capacity_ = size_;
  }
}

char* String::data() {
  return array_;
}

const char* String::data() const {
  return array_;
}

std::istream& operator>>(std::istream& in, String& str) {
  str.clear();
  char c;
  while (in.get(c) && !isspace(c)) {
    str.push_back(c);
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.length(); ++i) {
    out << str[i];
  }
  return out;
}

bool operator==(const String& a, const String& b) {
  if (a.length() != b.length()) {
    return false;
  }
  for (size_t i = 0; i < b.length(); ++i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const String& a, const String& b) {
  return !(a == b);
}

bool operator<(const String& a, const String& b) {
  for (size_t i = 0; i < a.length() + 1; ++i) {
    if (a[i] > b[i]) {
      return false;
    }
    if (a[i] < b[i]) {
      return true;
    }
  }
  return false;
}

bool operator>(const String& a, const String& b) {
  return b < a;
}

bool operator<=(const String& a, const String& b) {
  return !(b < a);
}

bool operator>=(const String& a, const String& b) {
  return !(a < b);
}

String operator+(const String a, const String b) {
  String result = a;
  result += b;
  return result;
}

String operator+(String a, const char b) {
  a.push_back(b);
  return a;
}

String operator+(char a, const String b) {
  String a_to_string(1, a);
  return a_to_string += b;
}
