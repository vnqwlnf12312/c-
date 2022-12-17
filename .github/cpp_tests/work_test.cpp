#include <sstream>
#include "../../matrix.h"
#include <cassert>
#include <utility>

template <int A, int B, typename F>
void print_Matrix (Matrix<A,B,F>& m)
{
    for (int i = 0 ; i < A ; i++)
    {
        for (int j = 0 ; j < B ; j++)
            std::cout << m[i][j].toString() << " ";
        std::cout << std::endl;
    }
}

int main()
{
    Residue<10000> not_simple(0);
    Residue<6571> simple(0);

    not_simple = not_simple + Residue<10000> (1);
    assert (not_simple == Residue<10000> (1) && "not_simple = not_simple(0) + Residue<10000> (1)");
    not_simple = not_simple - Residue<10000> (2);
    assert (not_simple == Residue<10000> (9999) && "not_simple = not_simple(1) - Residue<10000> (2)");
    ++not_simple;
    //not_simple += Residue<10000>(1);
    assert (not_simple == Residue<10000> (0) && "++not_simple");
    --not_simple;
    //not_simple -= Residue<10000>(1);
    not_simple *= Residue<10000>(5);
    Residue<10000> not_simple2 (5*9999);
    assert (not_simple == not_simple2 && "not_simple(9999) *= Residue<10000>(5); Residue<10000> not_simple2 (5*9999);");

    simple = simple + Residue<6571> (1);
    assert (simple == Residue<6571> (1) && "simple = simple(0) + Residue<6571> (1)");
    simple = simple - Residue<6571> (2);
    assert (simple == Residue<6571> (6570) && "simple = simple(1) - Residue<6571> (2)");
    //++simple;
    simple += Residue<6571>(1);
    assert (simple == Residue<6571> (0) && "++simple");
    //--simple;
    simple -= Residue<6571>(1);
    simple *= Residue<6571>(5);
    Residue<6571> simple2 (5*6570);
    assert (simple == simple2 && "simple(6570) *= Residue<6571>(5); Residue<6571> simple2 (5*6570);");
    simple2 /= Residue<6571> (5);
    assert (simple2 == Residue<6571>(6570) && "simple2(6566) /= Residue<6571> (5)");

    //--------------------------------------------------------------------------------------------------------------------------------
    
    vector<vector<int>> a = {{1,2,3,4,5}, 
                             {6,7,8,9,4}, 
                             {3,4,2,1,5}, 
                             {3,4,2,2,1}};
    vector<vector<int>> b = {{534,53 ,123,65 ,32}, 
                             {123,867,43 ,123,65}, 
                             {534,867,312,423,75}, 
                             {423,65 ,878,43 ,123}};
    Matrix<4,5> m1(a);

    auto m2 = m1 * Rational(5);
    Matrix<4,5> m3 (b);
    auto m4 = m3 + m1;
    auto m5 = m2.transposed();

    auto m6 = m5*m4;

    m6[4][4] = 1;
    assert (m6.det().toString() == "4111713081600562500" && 
"20990 40595 20070 11340 5315\\
29125 49940 26925 14690 6865\\
22815 45185 15870 11005 5355\\
23450 45475 15185 9890 5485\\
31510 40975 16420 15190 4925");
    auto m7 = m6.inverted();
    m7.rank();
    m7.trace();
    assert ((m6*m7 == Matrix<5,5>()) && "m6*m6^-1");

    m7[0][1] = 2;

    assert ((SquareMatrix<5>() == Matrix<5,5>()));
}