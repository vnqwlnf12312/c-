#include <deque>
#include <memory>
#include <cassert>
#include <vector>
#include <algorithm>
#include <type_traits>

#include "../../List.h"
#include "allocator_test.cpp"
#include "list_tests.cpp"


int main()
{
    allocator_test();

    list_test();
    list_iterator_test();
    list_allocator_test();
}