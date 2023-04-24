//  #include <memory>
//  #include <deque>
//  #include <cassert>
//  #include <vector>
//  #include "../../List.h"

void list_test()
{
    List<int> ll;

    List<int> llc (10);
    assert (llc.size() == 10);
    List<int> llc2 (10, 3);
    assert (*(++++llc2.begin()) == 3);

    for (int i =0 ; i<10'000; i++)
        ll.push_back(5);
    assert (ll.size() == 10'000);
    for (int i =0 ; i<10'000; i++)
        ll.push_front(5);
    ll.pop_back();
    assert (ll.size() == 19'999);

    for (int i =0 ; i<19'998; i++)
        ll.pop_back();
    assert (ll.size() == 1);

    ll.pop_back();
    assert (ll.size() == 0);
    ll.push_back(11);
    assert (ll.size() == 1);
    assert (*ll.begin() == 11);

    List<int> ll2 = ll;
    assert (ll2.size() == 1);
    ll2.push_back(1);
    ll2.push_back(1);
    ll2.push_back(2);
    assert (ll2.size() == 4 && ll.size() == 1);

    int count = 0;
    for (auto i: ll)
        count++;
    assert (count == 1);
    
    count = 0;
    for (auto i: ll2)
        count++;
    assert (count == 4);

    ll = ll2;
    assert (ll.size() == 4);

    ll.push_back(3);
    ll.push_back(4);
    ll.push_back(5);
    ll.push_back(3);
    ll.push_back(4);
    assert (ll.size() == 9);

    //std::remove (ll.begin(), ll.end(), 3);
    for (List<int>::const_iterator it = ll.begin(); it != ll.end();)
    {
        auto temp_it = it;
        ++it;
        if (*temp_it == 3) ll.erase(temp_it);
    }
    assert (ll.size() == 7);

    auto it = ll.begin();
    ++++it;
    ll.insert(it, 10);
    assert (*(++++++ll.begin()) == 10 || *(++++ll.begin()) == 10);
}

void list_iterator_test()
{
    using vec = std::vector<int>;
    List<vec> ll;

    vec vv = {1,2,3,4,5};
    for (int i =0; i < 10; i++)
        ll.push_back(vv);
    assert (ll.size() == 10);
    vv.push_back(1);
    assert (ll.begin()->size() == 5);

    for (auto &i: ll)
        i.push_back(2);

    auto it = ll.begin();
    ++++it;
    assert (it->back() == 2);


    auto it2 = it;
    List<vec>::const_iterator it3 = it2;
    static_assert (std::is_same_v <std::remove_reference_t<decltype(*it3)>, const vec>);
    
    List<vec>::const_iterator it4 = it3;
    static_assert (!std::is_constructible_v <List<vec>::iterator, List<vec>::const_iterator>);
}

template <typename T>
struct new_alloc
{
    using value_type = T;
    std::shared_ptr<std::vector<void*>> hash;

    new_alloc(){ hash = std::make_shared <std::vector<void*>> ();}
    new_alloc(const new_alloc& other): hash (other.hash) {}
    new_alloc& operator= (const new_alloc& other) {hash = other.hash; return *this;}

    template <typename U>
    new_alloc (const new_alloc<U>& other): hash (other.hash){}

    T* allocate (size_t size)
    {
        T* res = reinterpret_cast<T*> (new char[size*sizeof(T)]);
        hash->push_back(res);
        return res;
    }

    void deallocate (T* point, size_t size)
    {
        bool flag = false;
        for (auto i: *hash)
            if (i == point) flag = true;
        //assert (flag && "trying deallocate memory that is not own by allocator. Check operator =");

        std::remove (hash->begin(), hash->end(), point);
        delete[] reinterpret_cast<char*> (point);
    }

    bool operator== (const new_alloc& other) const
    {
        return hash == other.hash;
    }

    bool operator!= (const new_alloc& other) const
    {
        return !(*this == other);
    }

    auto select_on_container_copy_construction() const
    {
        return new_alloc();
    }

    using propagate_on_container_copy_assignment = std::true_type;

    // template <typename U>
    // struct rebind
    // {
    //     using other = new_alloc<U>;
    // };
    // template <class... Args >
    // void construct (T* p, Args&&... args)
    // {
    //     ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
    // }
    
    // void destroy (T* p)
    // {
    //     p->~T();
    // }
};

void list_allocator_test()
{
    List <int, new_alloc<int>> ll;
    new_alloc<int> alloc;
    
    List <int, new_alloc<int>> llc2 (alloc);
    List <int, new_alloc<int>> llc3 (10, alloc);
    assert (llc3.size() == 10);
    List <int, new_alloc<int>> llc4 (10, 4, alloc);
    assert (*(++++llc4.begin()) == 4);

    ll.push_back (1);
    ll.push_back (2);
    ll.push_back (3);
    ll.push_back (4);
    assert (ll.size() == 4);


    List <int, new_alloc<int>> ll2 (ll);
    assert (!(ll.get_allocator() == ll2.get_allocator()));
    ll2.pop_back();
    ll2.push_back(111);

    ll = ll2;
    assert (ll.get_allocator() == ll2.get_allocator());
}