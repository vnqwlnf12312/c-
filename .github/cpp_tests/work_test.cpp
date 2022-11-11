#include <sstream>
#include "../../BigInteger.h"
#include <cassert>

int main()
{
    BigInteger a;
    BigInteger b(1);
    if (b == false) assert ("Incorrect operator bool");

    a = 0;
    a += 1;
    a = a + 2;
    a = 3 - a;
    a = 10;
    a += b;
    a * b;
    a / b;
    -a;

    BigInteger c(a*b);
    a % b;
    b %= a;

    a == b;
    a != b;
    a < c;
    a > c;
    a <= c;
    a >= c;
    
    std::cout << c.toString() << std::endl;
    std::stringstream ss("123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789");
    ss >> b;
    std::cout << b << std::endl;
    b += 1;
    b++;
    ++b;
    b--;
    --b;

    Rational r1;
    Rational r2(5);
    Rational r3 (b);
    Rational r4 (b,1);
    r1 = 0;
    r1 += r2;
    r1 -= r2;
    r1 / r2;
    r1 * r2;
    -r1;

    r1 == r2;
    r1 != r2;
    r1 < r2;
    r1 <= r2;
    r1 > r2;
    r1 >= r2;
    r3.toString();
    r2.asDecimal(10);
    (double)r3;

    r2 = 0;
}