#include "../../deque.h"
#include <cassert>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

void print_vector (vector<int> v)
{
    for (auto i: v)
        cout << i << " ";
    cout << endl;
}


int main()
{
    Deque<int> dd;
    Deque<vector<int>> base(10);
    Deque<vector<int>> base2(10, vector<int>(5));


    Deque<vector<int>> dv;
    vector<int> vv = {1,2,3,4,5};

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
    assert (dv[5].back() != 3);
    for (auto &i: dv)
    {
        i.push_back(3);
    }


    assert (dv[5].back() == 3);
    assert (dv.size() == 22);

    vector<int> vv2 = {9,9,9};
    dv.push_back (vv2);

    auto it = dv.end();
    it------;------it;
    dv.erase (it);
    assert (dv.size() == 22);
    assert (dv[21] == vv2);

    vector<int>* v_link = &dv[21];
    vector<int> vv3 = {1}; 
    dv.push_back(vv3);
    dv.push_front(vv3);

    dv.pop_back();
    dv.pop_front();

    assert (*v_link == vv2);
    assert (*(--dv.end()) == vv2);
}