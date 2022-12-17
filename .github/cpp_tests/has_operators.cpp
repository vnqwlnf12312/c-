#include "../../matrix.h"
#include "cassert"
#include <optional>

bool test_sum(...)
{
    return false;
}

template <typename A, typename b = decltype (std::declval<A>() + 1)>
bool test_sum (const A& a)
{
    return true;
}

bool test_operator(...)
{
    return false;
}

template <typename A, typename b = decltype (std::declval<A>().operator+(1))>
bool test_operator (const A& a)
{
    return true;
}

int main ()
{
    assert(( std::is_same< SquareMatrix<1>, Matrix<1, 1> >::value));
    assert (!test_operator(Residue<10>(1)) && "int constructor should be explicit");
    assert (!test_sum(Residue<10>(1)) && "operator int should be explicit");
}