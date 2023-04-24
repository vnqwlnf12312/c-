// #include <memory>
// #include <deque>
// #include <cassert>
// #include <vector>
//#include "../../List.h"

const int stack_size = 5'000;

void allocator_test()
{
    using vec = std::vector<int, StackAllocator <int, stack_size>>;

    StackStorage <stack_size> storage1;
    StackStorage <stack_size> storage2;
    StackAllocator <int, stack_size> alloc (storage1);
    StackAllocator <int, stack_size> alloc2 (storage2);
    StackAllocator <vec, stack_size> alloc_vec (storage2);

    assert (alloc == alloc);
    assert (!(alloc == alloc2));
    assert (alloc != alloc2);

    std::deque<vec, StackAllocator <vec, stack_size>> dv (alloc_vec);
    
    vec vv (alloc);
    vv.push_back(1);
    vv.push_back(2);
    vv.push_back(3);
    vv.push_back(4);
    vv.push_back(5);

    dv.push_back (vv);
    dv.push_front (vv);
    vv[2] = 10;
    assert (dv[0][2] != 10);

    for (int i = 0 ; i < 10; i++)
    {
        dv.push_back(vv);
        dv.push_front(vv);
    }

    for (auto i: dv)
    {
        i.push_back(3);
    }
    assert (dv[4].back() != 3);
    for (auto &i: dv)
    {
        i.push_back(3);
    }

    assert (dv[5].back() == 3);
    assert (dv.size() == 22);

    vec vv2(alloc);
    vv2.push_back(9);
    vv2.push_back(9);
    vv2.push_back(9);

    dv.push_back (vv2);
    auto it = dv.end();
    it------;------it;
    dv.erase (it);
    assert (dv.size() == 22);
    assert (dv[21] == vv2);

    vec* v_link = &dv[21];

    vec vv3(alloc);
    vv3.push_back(1);

    dv.push_back(vv3);
    dv.push_front(vv3);

    dv.pop_back();
    dv.pop_front();

    assert (dv[21] == vv2);
    assert (*v_link == vv2);
    assert (*(--dv.end()) == vv2);
    
}