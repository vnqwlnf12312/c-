#include <sstream>
#include <new>
#include <iostream>
#include <cassert>

int number_of_new = 0;
size_t new_size;

void* operator new(std::size_t size) {
  ++number_of_new;
  new_size+= size;
  void *p = malloc(size);
  if(!p) throw std::bad_alloc();
  return p;
}

void* operator new  [](std::size_t size) {
  ++number_of_new;
  new_size+= size;
  void *p = malloc(size);
  if(!p) throw std::bad_alloc();
  return p;
}

void* operator new  [](std::size_t size, const std::nothrow_t&) {
  ++number_of_new;
  new_size+= size;
  return malloc(size);
}
void* operator new   (std::size_t size, const std::nothrow_t&) {
  ++number_of_new;
  new_size+= size;
  return malloc(size);
}


void operator delete(void* ptr) { free(ptr); }
void operator delete (void* ptr, const std::nothrow_t&) throw() { free(ptr); }
void operator delete[](void* ptr) { free(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) throw() { free(ptr); }

class BigInteger;
bool operator==(const BigInteger& first, int second);
bool operator<(const BigInteger& first, int second);
bool operator>(const BigInteger& first, int second);

#include "../../BigInteger.h"

bool operator==(const BigInteger& first, int second)
{
  number_of_new--;
  return first == BigInteger(second);
}

bool operator<(const BigInteger& first, int second)
{
  number_of_new--;
  return first < BigInteger(second);
}

bool operator>(const BigInteger& first, int second)
{
  number_of_new--;
  return first > BigInteger(second);
}


int main () {
  BigInteger b1;
  assert (number_of_new <= 1 && "BigInteger b1()");
  number_of_new = 0;

  BigInteger b2 (5);
  assert (number_of_new <= 1 && "BigInteger b2 (5)");
  number_of_new = 0;

  b1 = 2;
  assert (number_of_new <= 2 && "b1 = 2");
  number_of_new = 0;

  BigInteger b3 (b2);
  assert (number_of_new <= 1 && "BigInteger b3 (b1*b2)");
  number_of_new = 0;

  b2 = b1;
  assert (number_of_new <= 1 && "b2 = b1");
  b2 = 123456789;
  b2*= 100000000;
  b2*= 100000000;
  b2*= 100000000;
  b2*= 100000000;
  b2*= 100000000;
  number_of_new = 0;
  new_size = 0;

  b2+=1;
  int add_number = number_of_new;
  size_t prev_new_size = new_size;
  new_size = 0;
  number_of_new = 0;

  ++b2;
  assert (number_of_new <= add_number && new_size<=prev_new_size && "++b2");
  new_size = 0;
  number_of_new = 0;

  --b2;
  assert (number_of_new <= add_number && new_size<=prev_new_size && "--b2");
  new_size = 0;
  number_of_new = 0;
  
  b1 = b2;
  number_of_new = 0;
  prev_new_size = new_size;
  new_size = 0;

  b2++;
  assert (number_of_new <= 1+add_number && new_size < prev_new_size + 25  && "b2++");
  number_of_new = 0;
  new_size = 0;

  b2--;
  assert (number_of_new <= 1+add_number && new_size < prev_new_size + 25  && "b2--");
  b1 = 10;
  number_of_new = 0;
  new_size = 0;

  b2+=b1;
  assert (number_of_new <= add_number-1 && "b2+=b1");
  number_of_new = 0;

  b2-=b1;
  assert (number_of_new <= add_number-1 && "b2-=b1");
  number_of_new = 0;

  b3 = 0;
  number_of_new = 0;

  std::stringstream ss("123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789");
  ss >> b3;
  //assert (number_of_new <= 2 && "in >> b3");
  number_of_new = 0;

  b1+=b3;
  assert (number_of_new <= add_number && "b3 = dofiga; b1+=b3");
  b2 = 10;
  number_of_new = 0;

  b2-=b1;
  assert (number_of_new <= add_number && "b1 = dofiga; b2-=b1");
  number_of_new = 0;

  -b1;
  assert (number_of_new <= 1 && "-b1");
  number_of_new = 0;

  b1 + b2;
  assert (number_of_new <= add_number + 1 && "b1 + b2");
  number_of_new = 0;

  b1 - b2;
  assert (number_of_new <= add_number + 1 && "b1 - b2");
  number_of_new = 0;

  b1 == b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1 != b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1 < b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1 > b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1 <= b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1 >= b2;
  assert (number_of_new == 0 && "b1 - b2");
  number_of_new = 0;

  b1.toString();
  //std::cerr << number_of_new << std::endl;
  assert (number_of_new <= 2 && "b1.toString(); may be you should use String.reserve");
  std::stringstream s2;
  number_of_new = 0;

  std::cout << b1;
  assert (number_of_new <= 2 && "std::cout << b1");
  number_of_new = 0;

  if (b1);
  assert (number_of_new == 0 && "if (b1)");
  number_of_new = 0;

}