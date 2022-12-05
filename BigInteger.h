#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class BigInteger {
 public:
  explicit BigInteger(unsigned long long number); // ull чтобы в _bi помещалось ull, мещерин писал что оно должно помещаться туда

  BigInteger(long long number);

  explicit BigInteger(int number);

  BigInteger(const BigInteger& other) = default;

  explicit BigInteger(size_t size);

  explicit BigInteger(const std::string&);

  BigInteger();

  ~BigInteger() = default;

  friend bool operator<(const BigInteger&, const BigInteger&);

  friend std::istream& operator>>(std::istream&, BigInteger&);

  friend BigInteger operator "" _bi(unsigned long long);

  friend BigInteger operator "" _bi(const char*, size_t);

  explicit operator bool() const;

  bool operator==(const BigInteger& second) const;

  BigInteger& operator=(const BigInteger& second);

  BigInteger& operator+=(const BigInteger&);

  BigInteger& operator-=(const BigInteger&);

  BigInteger& operator*=(const BigInteger&);

  BigInteger& operator/=(const BigInteger&);

  BigInteger& operator%=(const BigInteger&);

  BigInteger operator-() const;

  BigInteger& operator--();

  BigInteger operator--(int);

  BigInteger& operator++();

  BigInteger operator++(int);

  std::string toString() const;

 private:

  void DeleteZeroes();

  BigInteger& MoveDigits(size_t n);

  bool SignIsUnchanged(const BigInteger&) const;

  bool AbsIsNotGreater(const BigInteger&, const BigInteger&, size_t);

  void Sum(const BigInteger&, long long, long long);

  std::vector<long long> digits_;
  const long long kBase_ = 100000000;
  bool is_negative_ = false;
};

BigInteger::BigInteger() : digits_{0} {
}

BigInteger::BigInteger(size_t size) {
  digits_.resize(size);
}

BigInteger::BigInteger(unsigned long long number) {
  if (number == 0) {
    digits_.push_back(0);
  }
  while (number > 0) {
    digits_.push_back(static_cast<long long>(number % kBase_));
    number /= kBase_;
  }
  DeleteZeroes();
}

BigInteger::BigInteger(long long number)
    : BigInteger(static_cast<unsigned long long>(std::abs(number))) {
  is_negative_ = number < 0;
}

BigInteger::BigInteger(int number)
    : BigInteger(static_cast<long long>(number)) {
}

void BigInteger::DeleteZeroes() {
  size_t size = digits_.size();
  for (size_t i = size; i > 1; --i) {
    if (digits_[i - 1] == 0) {
      digits_.pop_back();
    } else {
      return;
    }
  }
}

BigInteger& BigInteger::MoveDigits(size_t n) {
  if (n == 0) {
    return *this;
  }
  digits_.resize(digits_.size() + n);
  size_t new_size = digits_.size();
  for (size_t i = new_size; i > n; --i) {
    digits_[i - 1] = digits_[i - 1 - n];
  }
  for (size_t i = 0; i < n; ++i) {
    digits_[i] = 0;
  }
  DeleteZeroes();
  return *this;
}

bool BigInteger::AbsIsNotGreater(const BigInteger& first,
                                 const BigInteger& second,
                                 size_t second_start) {
  size_t first_pointer = first.digits_.size(),
      second_pointer = second.digits_.size();
  if (second_start + first.digits_.size() < second.digits_.size()) {
    return true;
  }
  if (second_start + first.digits_.size() > second.digits_.size()) {
    return false;
  }
  while (first_pointer > 0) {
    if (second.digits_[second_pointer - 1] < first.digits_[first_pointer - 1]) {
      return false;
    }
    if (second.digits_[second_pointer - 1] > first.digits_[first_pointer - 1]) {
      return true;
    }
    --first_pointer;
    --second_pointer;
  }
  return true;
}

bool operator<(const BigInteger& first, const BigInteger& second) {
  if (first.is_negative_ != second.is_negative_) {
    return first.is_negative_;
  }
  if (first.digits_.size() < second.digits_.size()) {
    return !first.is_negative_;
  }
  if (first.digits_.size() > second.digits_.size()) {
    return first.is_negative_;
  }
  for (size_t i = first.digits_.size(); i > 0; --i) {
    if (first.digits_[i - 1] < second.digits_[i - 1]) {
      return !first.is_negative_;
    }
    if (first.digits_[i - 1] > second.digits_[i - 1]) {
      return first.is_negative_;
    }
  }
  return false;
}

bool operator>(const BigInteger& first, const BigInteger& second) {
  return second < first;
}

bool operator<=(const BigInteger& first, const BigInteger& second) {
  return !(first > second);
}

bool operator>=(const BigInteger& first, const BigInteger& second) {
  return !(first < second);
}

BigInteger& BigInteger::operator=(const BigInteger& second) : digits_(second.digits_),
                                  kBase_(second.kBase_), is_negative_(second.is_negative_) {
  return *this;
}

bool BigInteger::operator==(const BigInteger& second) const {
  if (!*this && !second) {
    return true;
  }
  if (is_negative_ != second.is_negative_) {
    return false;
  }
  if (this->digits_.size() != second.digits_.size()) {
    return false;
  }
  for (size_t i = this->digits_.size(); i > 0; --i) {
    if (this->digits_[i - 1] != second.digits_[i - 1]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const BigInteger& first, const BigInteger& second) {
  return !(first == second);
}

std::ostream& operator<<(std::ostream& out, const BigInteger& bigint) {
  out << bigint.toString();
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& bigint) {
  std::string input;
  in >> input;
  BigInteger a(input);
  bigint = a;
  return in;
}

BigInteger::operator bool() const {
  return !digits_[0] == 0 && digits_.size() == 1;
}

bool is_same_sign(long long x, long long y) {
  return (x >= 0 && y >= 0) || (x < 0 && y < 0);
}

bool BigInteger::SignIsUnchanged(const BigInteger& second) const {
  if (digits_.size() > second.digits_.size()
      || !(is_negative_ ^ second.is_negative_)) {
    return true;
  }
  if (second.digits_.size() > digits_.size()) {
    return false;
  }
  for (size_t i = digits_.size(); i > 0; --i) {
    if (digits_[i - 1] != second.digits_[i - 1]) {
      return digits_[i - 1] > second.digits_[i - 1];
    }
  }
  return !is_negative_;
}

void BigInteger::Sum(const BigInteger& second,
                     long long sign1,
                     long long sign2) {
  size_t len_of_intersection = std::min(digits_.size(), second.digits_.size());
  bool first_is_smaller = digits_.size() < second.digits_.size();
  if (first_is_smaller) {
    digits_.resize(second.digits_.size());
  }
  size_t i = 0;
  long long result, to_remember = 0;
  for (; i < len_of_intersection; ++i) {
    result = digits_[i] * sign1 + second.digits_[i] * sign2 + to_remember;
    if (result < 0) {
      result += kBase_;
      to_remember = -1;
    } else {
      to_remember = 0;
    }
    digits_[i] = result % kBase_;
    to_remember += result / kBase_;
  }
  while ((to_remember != 0 || first_is_smaller) && i < digits_.size()) {
    if (first_is_smaller || sign1 == -1) {
      result = second.digits_[i] + to_remember;
    } else {
      result = digits_[i] + to_remember;
    }
    if (result < 0) {
      result += kBase_;
      to_remember = -1;
    } else {
      to_remember = 0;
    }
    digits_[i] = result % kBase_;
    to_remember += result / kBase_;
    ++i;
  }
  if (to_remember != 0) {
    digits_.push_back(abs(to_remember));
  }
  DeleteZeroes();
}

BigInteger& BigInteger::operator+=(const BigInteger& second) {
  if (is_negative_ == second.is_negative_) {
    Sum(second, 1, 1);
  } else {
    if (SignIsUnchanged(second)) {
      Sum(second, 1, -1);
    } else {
      Sum(second, -1, 1);
      is_negative_ = !is_negative_;
    }
  }
  DeleteZeroes();
  if (!*this) {
    is_negative_ = false;
  }
  return *this;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result += second;
}

BigInteger& BigInteger::operator-=(const BigInteger& second) {
  if (*this) {
    is_negative_ = !is_negative_;
  }
  *this += second;
  if (*this) {
    is_negative_ = !is_negative_;
  }
  return *this;
}

BigInteger operator-(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result -= second;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (!other || !*this) {
    digits_ = {0};
    is_negative_ = false;
    return *this;
  }
  if (other == 1) {
    return *this;
  }
  if (other == -1) {
    is_negative_ = !is_negative_;
    return *this;
  }
  size_t new_size = digits_.size() + other.digits_.size();
  BigInteger prod(new_size);
  prod.is_negative_ = is_negative_ ^ other.is_negative_;
  long long to_remember = 0;
  long long result;
  size_t first_factor, second_factor;
  for (size_t i = 0; i < new_size; ++i) {
    result = to_remember;
    to_remember = 0;
    if (i < other.digits_.size()) {
      first_factor = 0;
    } else {
      first_factor = i - other.digits_.size() + 1;
    }
    second_factor = i - first_factor + 1;
    while (first_factor < digits_.size() && second_factor > 0) {
      result += digits_[first_factor] * other.digits_[second_factor - 1];
      ++first_factor;
      --second_factor;
      to_remember += result / kBase_;
      result %= kBase_;
    }
    prod.digits_[i] = result;
  }
  prod.DeleteZeroes();
  *this = prod;
  return *this;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result *= second;
}

BigInteger& BigInteger::operator/=(const BigInteger& second) {
  if (digits_.size() < second.digits_.size()) {
    digits_ = {0};
    is_negative_ = false;
    return *this;
  }
  BigInteger answer(digits_.size() - second.digits_.size() + 1);
  size_t digit = answer.digits_.size();
  long long left_border, right_border, mid;
  BigInteger middle;
  long long to_remember;
  while (digit > 0) {
    to_remember = 0;
    left_border = 0;
    right_border = kBase_ - 1;
    while (right_border - left_border >= 0) {
      mid = (right_border + left_border) / 2;
      middle = mid;
      if (AbsIsNotGreater(middle *= second, *this, digit - 1)) {
        to_remember = mid;
        left_border = mid + 1;
      } else {
        right_border = mid - 1;
      }
    }
    answer.digits_[digit - 1] = to_remember;
    Sum((to_remember * second).MoveDigits(digit - 1), 1, -1);
    --digit;
  }
  answer.is_negative_ = is_negative_ ^ second.is_negative_;
  *this = answer;
  if (!*this) {
    is_negative_ = false;
  }
  DeleteZeroes();
  return *this;
}

BigInteger operator/(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result /= second;
}

BigInteger& BigInteger::operator%=(const BigInteger& second) {
  *this -= second * (*this / second);
  return *this;
}

BigInteger operator%(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result %= second;
}

BigInteger BigInteger::operator-() const {
  BigInteger result = *this;
  if (*this) {
    result.is_negative_ = !is_negative_;
  }
  return result;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger temp = *this;
  --*this;
  return temp;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger temp = *this;
  ++*this;
  return temp;
}

std::string BigInteger::toString() const {
  std::string answer;
  answer.reserve(digits_.size() * 8 + 1);
  if (is_negative_) {
    answer.push_back('-');
  }
  std::string digit;
  digit.reserve(9);
  digit = std::to_string(digits_[digits_.size() - 1]);
  answer += digit;
  for (size_t i = digits_.size() - 1; i > 0; --i) {
    digit = std::to_string(digits_[i - 1]);
    for (size_t j = 0; j + digit.length() < 8; ++j) {
      answer.push_back('0');
    }
    answer += digit;
  }
  return answer;
}

BigInteger::BigInteger(const std::string& string) {
  size_t end = 0;
  is_negative_ = false;
  while (string[end] == '-' || string[end] == '+') {
    if (string[end] == '-') {
      is_negative_ = !is_negative_;
    }
    ++end;
  }
  size_t size = string.length() - end;
  digits_.resize((size + 7) / 8);
  size_t cur = 0;
  for (size_t i = size + end; i >= end + 8; i -= 8) {
    digits_[cur] = std::stoll(string.substr(i - 8, 8));
    ++cur;
  }
  if (size % 8 != 0) {
    digits_[cur] = std::stoll(string.substr(end, size % 8));
  }
  if (!*this) {
    is_negative_ = false;
  }
  DeleteZeroes();
}

BigInteger operator "" _bi(unsigned long long number) {
  BigInteger big_int(number);
  return big_int;
}

BigInteger operator "" _bi(const char* input, size_t) {
  BigInteger big_int(input);
  if (input[0] == '-') {
    big_int.is_negative_ = true;
  }
  return big_int;
}

class Rational {
 private:

  BigInteger FindGCF();

  void Reduce();

  void CheckSigns();

  BigInteger numerator;
  BigInteger denominator;
  const long long kBase_ = 100000000;
 public:

  Rational() : numerator(1), denominator(1) {}

  Rational(const Rational& second) = default;

  Rational(const BigInteger& big_int) : numerator(big_int), denominator(1) {}

  Rational(const BigInteger& numerator, const BigInteger& denominator)
      : numerator(numerator), denominator(denominator) {
    CheckSigns();
  }

  Rational(const int& integer) : numerator(integer), denominator(1) {}

  Rational(const int& numerator, const int& denominator)
      : numerator(numerator), denominator(denominator) {
    CheckSigns();
  }

  friend bool operator<(const Rational&, const Rational&);

  Rational& operator=(const Rational&);

  Rational& operator+=(const Rational&);

  Rational& operator-=(const Rational&);

  Rational& operator*=(const Rational&);

  Rational& operator/=(const Rational&);

  Rational operator-();

  explicit operator double();

  std::string toString();

  std::string asDecimal(size_t);

  ~Rational() = default;
};

BigInteger Rational::FindGCF() {
  BigInteger first, second;
  bool sign_is_changed = false;
  if (numerator < 0) {
    numerator *= -1;
    sign_is_changed = true;
  }
  if (numerator > denominator) {
    first = numerator;
    second = denominator;
  } else {
    first = denominator;
    second = numerator;
  }
  BigInteger remainder = first % second;
  while (remainder != 0) {
    first = second;
    second = remainder;
    remainder = first % second;
  }
  if (sign_is_changed) {
    numerator *= -1;
  }
  return second;
}

void Rational::Reduce() {
  if (numerator == 0) {
    denominator = 1;
    return;
  }
  BigInteger GCF = FindGCF();
  numerator /= GCF;
  denominator /= GCF;
}

void Rational::CheckSigns() {
  if (denominator < 0) {
    denominator *= -1;
    numerator *= -1;
  }
}

Rational::Rational& operator=(const Rational& second) : numerator(second.numerator),
                              denominator(second.denominator),kBase_(second.kBase_) {
  return *this;
}

Rational& Rational::operator+=(const Rational& second) {
  numerator = numerator * second.denominator + denominator * second.numerator;
  denominator *= second.denominator;
  CheckSigns();
  Reduce();
  return *this;
}

Rational operator+(const Rational& first, const Rational& second) {
  Rational result = first;
  return result += second;
}

Rational& Rational::operator-=(const Rational& second) {
  *this *= -1;
  *this += second;
  *this *= -1;
  return *this;
}

Rational operator-(const Rational& first, const Rational& second) {
  Rational result = first;
  return result -= second;
}

Rational& Rational::operator*=(const Rational& second) {
  numerator *= second.numerator;
  denominator *= second.denominator;
  CheckSigns();
  Reduce();
  return *this;
}

Rational operator*(const Rational& first, const Rational& second) {
  Rational temp = first;
  return temp *= second;
}

Rational& Rational::operator/=(const Rational& second) {
  numerator *= second.denominator;
  denominator *= second.numerator;
  CheckSigns();
  Reduce();
  return *this;
}

Rational operator/(const Rational& first, const Rational& second) {
  Rational result = first;
  return result /= second;
}

Rational Rational::operator-() {
  Rational result = *this;
  result.numerator *= -1;
  return result;
}

bool operator<(const Rational& first, const Rational& second) {
  if (first.numerator < 0 && second.numerator > 0) {
    return true;
  }
  if (first.numerator > 0 && second.numerator < 0) {
    return false;
  }
  return (first.numerator * second.denominator)
      < (first.denominator * second.numerator);
}

bool operator>(const Rational& first, const Rational& second) {
  return second < first;
}

bool operator<=(const Rational& first, const Rational& second) {
  return !(second < first);
}

bool operator>=(const Rational& first, const Rational& second) {
  return !(first < second);
}

bool operator==(const Rational& first, const Rational& second) {
  return !(first < second || second < first);
}

bool operator!=(const Rational& first, const Rational& second) {
  return !(first == second);
}

std::string Rational::toString() {
  std::string answer;
  if (denominator == 1) {
    answer += numerator.toString();
    return answer;
  }
  answer += numerator.toString();
  answer.push_back('/');
  answer += denominator.toString();
  return answer;
}

std::string Rational::asDecimal(size_t precision) {
  Rational copy = *this;
  size_t divison_by_8 = precision / 8;
  for (size_t i = 0; i < divison_by_8; ++i) {
    copy.numerator *= kBase_;
  }
  for (size_t i = 0; i < precision % 8; ++i) {
    copy.numerator *= 10;
  }
  std::string answer = (copy.numerator / copy.denominator).toString();
  if (precision == 0) {
    return answer;
  }
  size_t start = 0;
  if (answer[0] == '-') {
    start = 1;
  }
  if (precision >= answer.length() + start) {
    if (answer[0] == '-') {
      answer = "-" + std::string(precision - answer.length() + 2, '0')
          + answer.substr(1, answer.length() - 1);
    } else {
      answer = std::string(precision - answer.length(), '0') + answer;
    }
  }
  answer.insert(answer.begin() + answer.length() - precision, '.');
  if (answer[0] == '.' || answer[1] == '.') {
    answer.insert(answer.begin() + start, '0');
  }
  return answer;
}

Rational::operator double() {
  return std::stod(asDecimal(30));
}
