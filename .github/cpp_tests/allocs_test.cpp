#include <new>
#include <iostream>
#include <cassert>

int number_of_new = 0;

void* operator new(std::size_t size) {
  ++number_of_new;
  void *p = malloc(size);
  if(!p) throw std::bad_alloc();
  return p;
}

void* operator new  [](std::size_t size) {
  ++number_of_new;
  void *p = malloc(size);
  if(!p) throw std::bad_alloc();
  return p;
}

void* operator new  [](std::size_t size, const std::nothrow_t&) {
  ++number_of_new;
  return malloc(size);
}
void* operator new   (std::size_t size, const std::nothrow_t&) {
  ++number_of_new;
  return malloc(size);
}


void operator delete(void* ptr) { free(ptr); }
void operator delete (void* ptr, const std::nothrow_t&) throw() { free(ptr); }
void operator delete[](void* ptr) { free(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) throw() { free(ptr); }

#include "../../String.cpp"

int main () {
  String s;
  assert (number_of_new <= 1 && "String s()");
  number_of_new = 0;

  String s1("abc");
  assert (number_of_new <= 1 && "String s(\"abc\")");
  number_of_new = 0;

  String s2(5, 'c');
  assert (number_of_new <= 1 && "String s(5, \'c\')");
  number_of_new = 0;

  String s3(s2);
  assert (number_of_new <= 1 && "String s(String other)");
  number_of_new = 0;

  s2 = s1;
  assert (number_of_new <= 1 && "s2=s1");
  number_of_new = 0;

  s3.push_back ('q');
  assert (number_of_new <= 1 && "push_back");
  number_of_new = 0;

  s3.pop_back();
  assert (number_of_new == 0 && "pop_back");
  number_of_new = 0;

  String s4 ("a");
  number_of_new = 0;
  s4 += s2;
  assert (number_of_new <= 1 && "s4+=s2");
  s4+=s2+=s2+=s2;
  s4 += "defe";
  String s5("def");
  number_of_new = 0;

  s4.find (s5);
  assert (number_of_new == 0 && "s4.find (s2)");
  number_of_new = 0;

  s4.rfind (s2);
  assert (number_of_new == 0 && "s4.rfind (s2)");
  number_of_new = 0;

  s4.substr (2,5);
  assert (number_of_new <= 1 && "s4.substr (2,5)");
  number_of_new = 0;
}