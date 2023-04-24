#include <iostream>
#include "../../deque.h"
#include <cassert>
#include <vector>


using std::vector;
using std::cout;
using std::endl;

struct Int {
    int x;
    Int() 
    {
        UpdateCounter();
        x = 0;
    }
    Int(int x) : x(x) {
        UpdateCounter();
    }
    Int(const Int& rhs) {
        UpdateCounter();
        x = rhs.x;
    }
    Int& operator=(const Int& rhs) {
        UpdateCounter();
        x = rhs.x;
        return *this;
    }
 
    Int(Int&&) = delete;
    Int& operator=(Int&&) = delete;
 
    static int counter;
    static int del_counter;
    static int max_value;
 
    static void Reset(int max_val) {
        counter = 0;
        del_counter = 0;
        max_value = max_val;
    }
 
    static void UpdateCounter() {
        ++counter;
        if (counter == max_value) {
            throw std::logic_error("Fail");
        }
    }
 
    bool operator==(const Int& rhs) const {
        return x == rhs.x;
    }

    ~Int()
    {
        ++del_counter;
    }
};
int Int::del_counter = 0;
int Int::counter = 0;
int Int::max_value = 0;


int main()
{
    Int::Reset (15);

    Deque<Int> ();
    assert (Int::counter == 0);

    Deque<Int> (3);
    assert (Int::counter == 3 || Int::counter == 4 && "Too many elements constructed");


    Int::Reset (5);
    try
    {
        Deque<Int> (6);
        assert (false && "Exception expected");
    }
    catch (...)
    {
        assert (Int::del_counter == 4 && "After exception wrong number of elements destroyed Deque<Int> (6)");
    }


    Int i (2);
    Int::Reset(10);
    try
    {
        Deque<Int> (11, i);
        assert (false && "Exception expected");
    }
    catch(...)
    {
        assert (Int::del_counter == 9 && "After exception wrong number of elements destroyed Deque<Int> (11, i)");
    }

    Deque<Int> d (5, i);
    Int::Reset(4);
    try
    {
        Deque<Int> d1 (d);
        assert (false && "Exception expected");
    }
    catch(...)
    {
        assert (Int::del_counter == 3 && "After exception wrong number of elements destroyed Deque<Int> d1 (d)");
    }


    Int::Reset(2);
    try
    {
        d.push_back (i);
        d.push_back (i);
        assert (false && "Exception expected");
    }
    catch (...)
    {
        assert (d.size() == 6 && "After bad push_back size should not changed");
        d.push_back (i);
        d.pop_back ();
    }
    
    Int::Reset(2);
    try
    {
        d.push_front (i);
        d.push_front (i);
        assert (false && "Exception expected");
    }
    catch (...)
    {
        assert (d.size() == 7 && "After bad push_back size should not changed");
        d.push_back (i);
        d.pop_back ();
    }

    cout << "kar";
}