#include <iostream>
#include <cstring>
class String {
private:
  size_t sz = 0;
  size_t cap = 1;
  char* array;
  void enlarge_cap() {
    cap *= 2;
    char* to_delete = array;
    array = new char[cap + 2];
    memcpy(array, to_delete, sz + 1);
    delete to_delete;
  }
  bool is_equal(const String& substring, size_t i) const {
    for (size_t j = 0; j < substring.sz; ++j) {
      if (array[i + j] != substring.array[j]) {
        return false;
      }
    }
    return true;
  }
  String(size_t n) : sz(0), cap(n), array(new char[n + 1]) {
    array[0] = '\0';
  }
public:
  String(const char* str): sz(strlen(str)), cap(sz), array(new char[sz + 1]){
    memcpy(array, str, sz + 1);
  }
  String(const size_t n,  const char c): sz(n), cap(n), array(new char[n + 1]) {
    memset(array, c, n);
    array[n] = '\0';
  }
  String(const String& str): sz(str.sz), cap(str.cap), array(new char[str.cap + 1]) {
    memcpy(array, str.array, sz + 1);
  }
  String(): sz(0), cap(0), array(new char[1]){
    array[0] = '\0';
  }
  void swap(String& str) {
    std::swap(array, str.array);
    std::swap(sz, str.sz);
    std::swap(cap, str.cap);
  }
  size_t length() const {
    return sz;
  }
  size_t size() const {
    return sz;
  }
  size_t capacity() const {
    return cap;
  }
  char& operator[](size_t index) {
    return array[index];
  }
  const char& operator[](size_t index) const {
    return array[index];
  }
  void push_back(const char a) {
    if (sz >= cap) {
      enlarge_cap();
    }
    array[sz] = a;
    array[sz + 1] = '\0';
    ++sz;
  }
  void pop_back() {
    --sz;
  }
  char& front() {
    return array[0];
  }
  const char& front() const {
    return array[0];
  }
  char& back() {
    return array[sz - 1];
  }
  const char& back() const {
    return array[sz - 1];
  }
  String& operator=(String str) {
    swap(str);
    return *this;
  }
  String& operator+=(const String& str) {
    if (cap < sz + str.sz) {
      char* to_delete = array;
      array = new char[sz + str.sz + 1];
      cap = sz + str.sz;
      memcpy(array, to_delete, sz);
      delete to_delete;
    }
    memcpy(array + sz, str.array, str.sz + 1);
    sz += str.sz;
    return *this;
  }
  String& operator+=(const char a) {
    push_back(a);
    return *this;
  }
  size_t find(const String& substring) const {
    size_t end_of_search = sz - substring.sz;
    for (size_t i = 0; i < end_of_search; ++i) {
      if (is_equal(substring, i)) {
        return i;
      }
    }
    return length();
  }
  size_t rfind(const String& substring) const {
    for (size_t i = sz - substring.sz + 1; i >= 1; --i) {
      if (is_equal(substring, i - 1)) {
        return i - 1;
      }
    }
    return length();
  }
  String substr(size_t start, size_t count) const {
    String to_return(count);
    memcpy(to_return.array, array + start, sz);
    to_return[count] = '\0';
    to_return.sz = count;
    to_return.cap = count;
    return to_return;
  }
  bool empty() const {
    return sz == 0;
  }
  void clear() {
    sz = 0;
  }
  void shrink_to_fit() {
    if (sz < cap) {
      char* to_delete = array;
      array = new char[sz + 1];
      memcpy(array, to_delete, sz + 1);
      delete to_delete;
      cap = sz;
    }
  }
  char* data() {
    return array;
  }
  const char* data() const {
    return array;
  }
  ~String() {
    delete[] array;
  }
};
std::istream& operator>>(std::istream& in, String& str) {
  str.clear();
  char c;
  while(in.get(c) && !isspace(c)) {
    str.push_back(c);
  }
  return in;
}
std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.length(); ++i) {
    out<<str[i];
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
