#pragma once

#include "../../SharedPtr.h"

// #include <memory>
// template<typename T>
// using SharedPtr = std::shared_ptr<T>;

// template<typename T>
// using WeakPtr = std::weak_ptr<T>;

// template<typename T>
// using EnableSharedFromThis = std::enable_shared_from_this<T>;

// template <typename T, typename... Args>
// SharedPtr<T> makeShared(Args&&... args) {
//     return std::make_shared<T>(std::forward<Args>(args)...);
// }

// template <typename T, typename Alloc, typename... Args>
// SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
//     return std::allocate_shared<T>(std::forward<const Alloc>(alloc), std::forward<Args>(args)...);
// }


#include <cassert>
#include <iostream>
#include <vector>

struct First
{
    int a = 0;
};

struct Second: First
{
    int c = 0;
    int b = 0;
};

struct In
{
    int a = 0;
    In(){}
    In(int in):a(in){}
    In(const In& other):a(other.a){}
};


struct del;
int two_dels = 0;
int one_dels = 0;

struct delOneLove
{
    friend del;
protected:
    ~delOneLove(){one_dels += 1;}
public:
    delOneLove (){}
    delOneLove (const delOneLove& other){}
};

struct delTwoLove: delOneLove
{
    friend del;
private:
    ~delTwoLove(){two_dels += 1;}
public:
    int a = 0;
    delTwoLove (){}
    delTwoLove (const delOneLove& other){}
};

struct del
{
    void operator()(delOneLove* obj) {
        obj->~delOneLove();
        std::free (obj);    
    }

    void operator()(delTwoLove* obj)
    {
        obj->~delTwoLove();
        std::free (obj);
    }
};