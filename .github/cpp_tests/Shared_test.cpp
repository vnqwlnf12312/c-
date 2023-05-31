#include "util_structures.h"

void shared_test()
{
    
    SharedPtr <First> f1 (new First{1});
    SharedPtr <First> f2 (new Second{3,2,1});
    assert (f1->a == 1);    
    assert (f2->a == 3);

    SharedPtr <First> f3 = f2;
    assert (f3.use_count() == f2.use_count());
    assert (f3->a == 3);
    assert (f2->a == 3); 

    f2 = f1;
    assert (f1.use_count() == f2.use_count());
    assert (f1->a == 1);
    assert (f2->a == 1);
    assert (f3->a == 3);
    assert (f3.use_count() == 1);

    f1 = f3;
    assert (f2.use_count() == 1);
    assert (f1->a == 3);

    SharedPtr <Second> s1 (new Second{30,20,10});
    f1 = s1;
    assert (f1->a == 30);
    assert (f1.use_count() == 2);
    assert (s1.use_count() == 2);
    f1->a = 15;
    assert (s1->a == 15);

    SharedPtr <Second> s2 = std::move(s1);
    assert (s2->a == 15 && s2->c == 20 && s2->b == 10);
    assert (f1.use_count() == 2);
    
    f3 = std::move (s2);
    assert (f1.use_count() == 2);
    assert (f3->a == 15);

    First* fs = new First{3};
    f1.reset (fs);
    assert (f1->a == 3);
    assert (f1.use_count() == 1);
    assert (f3->a == 15);
    assert (f3.use_count() == 1);

    Second* ss = new Second {6,7,8};
    f1.reset(ss);
    assert (f1->a == 6);
    assert (f1.use_count() == 1);


    
    {
        SharedPtr <std::vector<int>> trash;
    }

    f1 = makeShared<First> ();
    assert (f1->a == 0);
    assert (f1.use_count() == 1);

    auto a1 = makeShared<In> (5);
    assert (a1->a == 5);
    assert (a1.use_count() == 1);

    auto a2 = a1;
    assert (a1->a == 5);
    assert (a2->a == 5);
    assert (a1.use_count() == 2);

    
    
    {
        delTwoLove* dd = new delTwoLove();
        del d;

        auto d1 = SharedPtr<delOneLove> (dd, d);
    }
    assert (one_dels == 1);
    assert (two_dels == 1);
    
}