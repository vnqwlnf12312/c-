#include <iostream>
#include "../../List.h"
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
            throw std::logic_error("Fail on max value " + std::to_string(counter));
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
    Int temp = 2;
    Int::Reset (2);

    List<Int> ll;
    ll.push_back(temp);

    Int::Reset (10);
    for (int i=0; i <9; i++)
        ll.push_back(temp);
    assert (ll.size() == 10);
    
    bool isThrow = true;
    try
    {
        ll.push_back(temp);
        isThrow = false;
    }
    catch (const std::logic_error& e)
    {
        assert (ll.size() == 10);
        assert (Int::del_counter == 0);
    }
    assert (isThrow && "Why exception is not throw??? push_back *Travolta meme*");

    Int::Reset (2);
    try
    {
        ll.push_front(temp);
        ll.push_front(temp);
        isThrow = false;
    }
    catch (const std::logic_error& e)
    {
        assert (ll.size() == 11);
        assert (Int::del_counter == 0);
    }
    assert (isThrow && "Why exception is not throw??? push_front *Travolta meme*");

    ll.pop_back();
    assert (Int::del_counter == 1);
    ll.pop_front();
    assert (Int::del_counter == 2);

    Int::Reset (2);
    try
    {
        auto it = ll.begin();
        ++++it;
        ll.insert (it, temp);
        ll.insert (it, temp);
        isThrow = false;
    }
    catch (const std::logic_error& e)
    {
        assert (ll.size() == 10);
        assert (Int::del_counter == 0);
    }
    assert (isThrow && "Why exception is not throw??? insert *Travolta meme*");

    auto it = ll.begin();
    ++++it;
    ll.erase (it);
    assert (ll.size() == 9);
    assert (Int::del_counter == 1);

    Int::Reset(3);
    try
    {
        List<Int> ll1(ll);
        isThrow = false;
    }
    catch(const std::logic_error& e)
    {
        ll.size() == 9;
    }
    assert (isThrow && "Why exception is not throw??? copy construct *Travolta meme*");
    
    List<Int> ll2;
    ll2.push_back(1);
    ll2.push_back(1);
    ll2.push_back(1);

    Int::Reset(4);
    try 
    {
        ll2 = ll;
        isThrow = false;
    }
    catch(const std::logic_error& e)
    {
        ll.size() == 9;
        ll2.size() == 3;
    }
    assert (isThrow && "Ммм, исключение? Да и чёрт с ним, скопируем остальное) operator=");

    Int::Reset(4);
    try 
    {
        List<Int> ll1 (5);
        isThrow = false;
    }
    catch(const std::logic_error& e)
    {
        assert (Int::del_counter == 3);
    }
    assert (isThrow && "Why exception is not throw??? constructor from number *Travolta meme*");

    cout << "kar";
}