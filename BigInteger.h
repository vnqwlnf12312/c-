#include <algorithm>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>

class BigInteger {
 public:
  explicit BigInteger(unsigned long long number);

  explicit BigInteger(int number);

  BigInteger(long long number);

  BigInteger(const BigInteger& other) = default;

  BigInteger();

  ~BigInteger() = default;

  friend bool operator<(const BigInteger&, const BigInteger&);

  friend std::istream& operator>>(std::istream&, BigInteger&);

  friend BigInteger operator "" _bi(unsigned long long);

  friend BigInteger operator "" _bi(const char*, size_t);

  explicit operator bool() const;

  bool operator==(const BigInteger& second) const;

  BigInteger& operator=(const BigInteger& second) = default;

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
  explicit BigInteger(size_t size);

  explicit BigInteger(const std::string&);

  void DeleteZeroes();

  BigInteger& MoveDigits(size_t n);

  bool sign_is_unchanged(const BigInteger&) const;

  bool AbsIsNotGreater(const BigInteger&, const BigInteger&, size_t);

  void Sum(const BigInteger&, long long, long long);

  std::vector<long long> digits;
  long long base = 100000000;
  bool is_negative = false;
};

BigInteger::BigInteger() : digits{0} {
}

BigInteger::BigInteger(size_t size) {
  digits.resize(size);
}

BigInteger::BigInteger(unsigned long long number) {
  if (number == 0) {
    digits.push_back(0);
  }
  while (number > 0) {
    digits.push_back(static_cast<long long>(number % base));
    number /= 100000000;
  }
  DeleteZeroes();
}

BigInteger::BigInteger(long long number)
    : BigInteger(static_cast<unsigned long long>(std::abs(number))) {
  is_negative = number < 0;
}

BigInteger::BigInteger(int number)
    : BigInteger(static_cast<long long>(number)) {
}

void BigInteger::DeleteZeroes() {
  size_t size = digits.size();
  for (size_t i = size; i > 1; --i) {
    if (digits[i - 1] == 0) {
      digits.pop_back();
    } else {
      return;
    }
  }
}

BigInteger& BigInteger::MoveDigits(size_t n) {
  if (n == 0) {
    return *this;
  }
  digits.resize(digits.size() + n);
  size_t new_size = digits.size();
  for (size_t i = new_size; i > n; --i) {
    digits[i - 1] = digits[i - 1 - n];
  }
  for (size_t i = 0; i < n; ++i) {
    digits[i] = 0;
  }
  DeleteZeroes();
  return *this;
}

bool BigInteger::AbsIsNotGreater(const BigInteger& first,
                                 const BigInteger& second,
                                 size_t second_start) {
  size_t first_pointer = first.digits.size(),
      second_pointer = second.digits.size();
  if (second_start + first.digits.size() < second.digits.size()) {
    return true;
  }
  if (second_start + first.digits.size() > second.digits.size()) {
    return false;
  }
  while (first_pointer > 0) {
    if (second.digits[second_pointer - 1] < first.digits[first_pointer - 1]) {
      return false;
    }
    if (second.digits[second_pointer - 1] > first.digits[first_pointer - 1]) {
      return true;
    }
    --first_pointer;
    --second_pointer;
  }
  return true;
}

bool operator<(const BigInteger& first, const BigInteger& second) {
  if (first.is_negative != second.is_negative) {
    return first.is_negative;
  }
  if (first.digits.size() < second.digits.size()) {
    return !first.is_negative;
  }
  if (first.digits.size() > second.digits.size()) {
    return first.is_negative;
  }
  for (size_t i = first.digits.size(); i > 0; --i) {
    if (first.digits[i - 1] < second.digits[i - 1]) {
      return !first.is_negative;
    }
    if (first.digits[i - 1] > second.digits[i - 1]) {
      return first.is_negative;
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

bool BigInteger::operator==(const BigInteger& second) const {
  if (!*this && !second) {
    return true;
  }
  if (this->is_negative != second.is_negative) {
    return false;
  }
  if (this->digits.size() != second.digits.size()) {
    return false;
  }
  for (size_t i = this->digits.size(); i > 0; --i) {
    if (this->digits[i - 1] != second.digits[i - 1]) {
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
  if (digits[0] == 0 && digits.size() == 1) {
    return false;
  }
  return true;
}

bool is_same_sign(long long x, long long y) {
  if ((x >= 0 && y >= 0) || (x < 0 && y < 0)) {
    return true;
  }
  return false;
}

bool BigInteger::sign_is_unchanged(const BigInteger& second) const {
  if (digits.size() > second.digits.size()
      || !(is_negative ^ second.is_negative)) {
    return true;
  }
  if (second.digits.size() > digits.size()) {
    return false;
  }
  for (size_t i = digits.size(); i > 0; --i) {
    if (digits[i - 1] != second.digits[i - 1]) {
      return digits[i - 1] > second.digits[i - 1];
    }
  }
  return !is_negative;
}

void BigInteger::Sum(const BigInteger& second,
                     long long sign1,
                     long long sign2) {
  size_t len_of_intersection = std::min(digits.size(), second.digits.size());
  bool first_is_smaller = digits.size() < second.digits.size();
  if (first_is_smaller) {
    digits.resize(second.digits.size());
  }
  size_t i = 0;
  long long result, to_remember = 0;
  for (; i < len_of_intersection; ++i) {
    result = digits[i] * sign1 + second.digits[i] * sign2 + to_remember;
    if (result < 0) {
      result += base;
      to_remember = -1;
    } else {
      to_remember = 0;
    }
    digits[i] = result % base;
    to_remember += result / base;
  }
  while ((to_remember != 0 || first_is_smaller) && i < digits.size()) {
    if (first_is_smaller || sign1 == -1) {
      result = second.digits[i] + to_remember;
    } else {
      result = digits[i] + to_remember;
    }
    if (result < 0) {
      result += base;
      to_remember = -1;
    } else {
      to_remember = 0;
    }
    digits[i] = result % base;
    to_remember += result / base;
    ++i;
  }
  if (to_remember != 0) {
    digits.push_back(abs(to_remember));
  }
  DeleteZeroes();
}

BigInteger& BigInteger::operator+=(const BigInteger& second) {
  if (is_negative == second.is_negative) {
    Sum(second, 1, 1);
  } else {
    if (sign_is_unchanged(second)) {
      Sum(second, 1, -1);
    } else {
      Sum(second, -1, 1);
      is_negative = !is_negative;
    }
  }
  DeleteZeroes();
  if (!*this) {
    is_negative = false;
  }
  return *this;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result += second;
}

BigInteger& BigInteger::operator-=(const BigInteger& second) {
  if (*this) {
    is_negative = !is_negative;
  }
  *this += second;
  if (*this) {
    is_negative = !is_negative;
  }
  return *this;
}

BigInteger operator-(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  return result -= second;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (!other || !*this) {
    digits = {0};
    is_negative = false;
    return *this;
  }
  if (other == 1) {
    return *this;
  }
  if (other == -1) {
    is_negative = !is_negative;
    return *this;
  }
  size_t new_size = digits.size() + other.digits.size();
  BigInteger prod(new_size);
  prod.is_negative = is_negative ^ other.is_negative;
  long long to_remember = 0;
  long long result;
  size_t first_factor, second_factor;
  for (size_t i = 0; i < new_size; ++i) {
    result = to_remember;
    to_remember = 0;
    if (i < other.digits.size()) {
      first_factor = 0;
    } else {
      first_factor = i - other.digits.size() + 1;
    }
    second_factor = i - first_factor + 1;
    while (first_factor < digits.size() && second_factor > 0) {
      result += digits[first_factor] * other.digits[second_factor - 1];
      ++first_factor;
      --second_factor;
      to_remember += result / base;
      result %= base;
    }
    prod.digits[i] = result;
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
  if (digits.size() < second.digits.size()) {
    digits = {0};
    is_negative = false;
    return *this;
  }
  BigInteger answer(digits.size() - second.digits.size() + 1);
  size_t digit = answer.digits.size();
  long long left_border, right_border, mid;
  BigInteger middle;
  long long to_remember;
  while (digit > 0) {
    to_remember = 0;
    left_border = 0;
    right_border = base - 1;
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
    answer.digits[digit - 1] = to_remember;
    Sum((to_remember * second).MoveDigits(digit - 1), 1, -1);
    --digit;
  }
  answer.is_negative = is_negative ^ second.is_negative;
  *this = answer;
  if (!*this) {
    is_negative = false;
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
    result.is_negative = !is_negative;
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
  if (is_negative) {
    answer.push_back('-');
  }
  std::string digit;
  digit = std::to_string(digits[digits.size() - 1]);
  answer += digit;
  for (size_t i = digits.size() - 1; i > 0; --i) {
    digit = std::to_string(digits[i - 1]);
    for (size_t j = 0; j + digit.length() < 8; ++j) {
      answer.push_back('0');
    }
    answer += digit;
  }
  return answer;
}

BigInteger::BigInteger(const std::string& string) {
  size_t end = 0;
  is_negative = false;
  while (string[end] == '-' || string[end] == '+') {
    if (string[end] == '-') {
      is_negative = !is_negative;
    }
    ++end;
  }
  size_t size = string.length() - end;
  digits.resize((size + 7) / 8);
  size_t cur = 0;
  for (size_t i = size + end; i >= end + 8; i -= 8) {
    digits[cur] = std::stoll(string.substr(i - 8, 8));
    ++cur;
  }
  if (size % 8 != 0) {
    digits[cur] = std::stoll(string.substr(end, size % 8));
  }
  if (!*this) {
    is_negative = false;
  }
  DeleteZeroes();
}

BigInteger operator "" _bi(unsigned long long number) {
  BigInteger big_int = number;
  return big_int;
}

BigInteger operator "" _bi(const char* input, size_t) {
  BigInteger big_int(input);
  if (input[0] == '-') {
    big_int.is_negative = true;
  }
  return big_int;
}

class Rational {
  BigInteger numerator;
  BigInteger denominator;

  BigInteger FindGCF();

  void reduce();

  void CheckSigns();

  long long base = 100000000;
 public:
  friend bool operator<(const Rational&, const Rational&);

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

  Rational& operator=(const Rational&) = default;

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

void Rational::reduce() {
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

Rational& Rational::operator+=(const Rational& second) {
  numerator = numerator * second.denominator + denominator * second.numerator;
  denominator *= second.denominator;
  CheckSigns();
  reduce();
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
  reduce();
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
  reduce();
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
    copy.numerator *= base;
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
