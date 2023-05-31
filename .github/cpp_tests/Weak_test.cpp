#include "util_structures.h"

template <typename shared, typename pointer>
void small_weak_test (shared& s, int count = 1)
{
    {
        WeakPtr<pointer> w1 = s;
        assert (w1.use_count() == count);
        assert (!w1.expired());

        WeakPtr<pointer> w2 = s;
        assert (w2.use_count() == count);
        assert (!w2.expired());

        w2 = w1;
        w1 = w2;
    }

    WeakPtr<pointer> w1 = s;
    assert (w1.use_count() == count);
    assert (!w1.expired());

    {
        auto s2 = w1.lock();
        assert (s2.use_count() == count + 1);
        assert (!w1.expired());
        assert (w1.use_count() == count + 1);
    }

    w1.use_count() == count;
}

void weak_test()
{
    First* ff = new First {1};
    SharedPtr<First> s1 (ff);
    small_weak_test <SharedPtr<First>, First> (s1);
    assert (s1.use_count() == 1);

    
    Second* ss = new Second {3,2,1};
    SharedPtr<Second> s2 (ss);

    small_weak_test <SharedPtr<Second>, Second> (s2);
    assert (s1.use_count() == 1);
    assert (s2.use_count() == 1);

    small_weak_test <SharedPtr<Second>, First> (s2);
    assert (s2.use_count() == 1);

    s1 = s2;
    small_weak_test <SharedPtr<First>, First> (s1, 2);
    assert (s1.use_count() == 2);
    assert (s2.use_count() == 2);

    auto s3 = makeShared <In> (1);
    small_weak_test <SharedPtr<In>, In> (s3);
    assert (s3.use_count() == 1);
    assert (s3->a == 1);

    delOneLove* dd = new delOneLove();
    del d;
    SharedPtr <delOneLove> s4 (dd, d);
    small_weak_test <SharedPtr<delOneLove>, delOneLove> (s4);
    assert (s4.use_count() == 1);
}