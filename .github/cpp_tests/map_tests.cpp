#include <memory>
#include <deque>
#include <cassert>
#include <vector>
#include "../../Unordered_map.h"
#include <string>
//#include <unordered_map>

// template <typename Key, typename Value, typename Hash = std::hash<Key>,
//          typename EqualTo = std::equal_to<Key>,
//          typename Alloc = std::allocator<std::pair<const Key, Value>> >
// using UnorderedMap = std::unordered_map<Key, Value, Hash, EqualTo, Alloc>;

void map_test()
{
    UnorderedMap <std::string, int> map;
    map ["a"] = 3;
    map ["b"] = 4;
    auto map2 = map;
    assert (map2["a"] == 3);
    
    map2["c"] = 1;
    map = map2;
    assert (map["c"] == 1);
    assert (map.size() == 3 && map2.size() == 3);
    map2 ["aa"] = 10;

    map = std::move (map2);
    assert (map.size() == 4 && map2.size() == 0);
    assert (map["aa"] == 10);
    map = std::move (UnorderedMap <std::string, int>());

    //Хочешь, я расскажу тебе, что такое безумие
    for (int i = 0; i < 10'000; i++)
    {
        map["bc"] = 42;
        assert (map["bc"] == 42 && map.size() == 1);
    }

    bool isThrow = true;
    try
    {
        map.at ("qwerty");
        isThrow = false;
    }
    catch (std::exception e)
    {
        map.size() == 1;
    }
    if (!isThrow) assert ("method at lose exception((((");

    const auto const_map = map;
    try
    {
        const_map.at ("qwerty");
        isThrow = false;
    }
    catch (std::exception e)
    {
        const_map.size() == 1;
    }

    map = std::move(const_map);
    assert (const_map.size() == 1);

    map ["abc"]++;
    assert (map.size() == 2);
    assert (map["abc"] == 1);

    map.emplace ("ccc", 321);
    assert (map.size() == 3);
    assert (map["ccc"] == 321);

    map.emplace ("ccc", 123);
    assert (map.size() == 3);
    assert (map["ccc"] == 321);

    map.insert (std::pair(std::string("123"),123));
    assert (map.size() == 4);
    assert (map["123"] == 123);

    map.insert (std::pair("123",321));
    assert (map.size() == 4);
    assert (map["123"] == 123);

    std::vector <std::pair <std::string, int>> vec;
    for (int i = 1; i < 11; i++)
        vec.push_back ({std::to_string(i), i});

    auto it = vec.begin();
    it+=6;

    map.insert (vec.begin(), it);
    assert (map["3"] == 3);
    assert (map.size() == 10);
    
}

using vec = std::vector<int>;

struct vecHash {
    size_t operator()(const vec& value) const {
        long long hash = 0;
        for (auto i: value)
            hash += i;
        return std::hash<long long >()(hash);
    }
};

void map_iterator_test()
{
    UnorderedMap <vec, std::string, vecHash> map;

    vec vv = {1,2,3,4,5};
    map [vv] = "aaa";

    assert (map.size() == 1);
    assert (map [vv] == "aaa");

    for (int i =0; i < 10; i++)
    {
        vv[2] += 1;
        map [vv] = "aaa" + std::to_string (vv[2]);
    }

    assert (map [vec({1,2,5,4,5})] == "aaa5");
    assert (map.size() == 11);

    for (auto &i : map)
    {
        assert (i.first.size() == 5);
        assert (i.second.find("aaa") == 0);
    }

    auto it = map.begin();
    ++++++it++++++;
    map.erase (it);
    assert (map.size() == 10);

    using val = std::pair<vec, std::string>;
    std::vector <val> v_map;
    v_map.insert (v_map.begin(), map.begin(), map.end());
    assert (v_map.size() == 10);

    for (auto i: v_map)
    {
        assert (i.first.size() == 5);
        assert (i.second.find("aaa") == 0);
    }

    v_map.clear();
    assert (map.size() == 10);
    assert (map [vec ({1,2,3,4,5})] == "aaa");
    for (auto &i : map)
    {
        assert (i.first.size() == 5);
        assert (i.second.find("aaa") == 0);
    }

    v_map.insert (v_map.begin(), map.cbegin(), map.cend());
    assert (v_map.size() == 10);

    for (auto i: v_map)
    {
        assert (i.first.size() == 5);
        assert (i.second.find("aaa") == 0);
    }
    
    auto it2 = it;
    decltype (map.cbegin()) it3 = it2;
    static_assert (std::is_same_v <std::remove_reference_t<decltype(*it3)>, const std::pair<const vec, std::string>>);
    
    decltype (map.cbegin()) it4 = it3;
    static_assert (!std::is_constructible_v <decltype (map.begin()), decltype (map.cbegin())>);
}



// template <typename T>
// struct new_alloc
// {
//     using value_type = T;
//     std::shared_ptr<std::vector<void*>> hash;

//     new_alloc(){ hash = std::make_shared <std::vector<void*>> ();}
//     new_alloc(const new_alloc& other): hash (other.hash) {}
//     new_alloc& operator= (const new_alloc& other) {hash = other.hash; return *this;}

//     template <typename U>
//     new_alloc (const new_alloc<U>& other): hash (other.hash){}

//     T* allocate (size_t size)
//     {
//         T* res = reinterpret_cast<T*> (new char[size*sizeof(T)]);
//         hash->push_back(res);
//         return res;
//     }

//     void deallocate (T* point, size_t size)
//     {
//         bool flag = false;
//         for (auto i: *hash)
//             if (i == point) flag = true;
//         //assert (flag && "trying deallocate memory that is not own by allocator. Check operator =");

//         std::remove (hash->begin(), hash->end(), point);
//         delete[] reinterpret_cast<char*> (point);
//     }

//     bool operator== (const new_alloc& other) const
//     {
//         return hash == other.hash;
//     }

//     bool operator!= (const new_alloc& other) const
//     {
//         return !(*this == other);
//     }

//     auto select_on_container_copy_construction() const
//     {
//         return new_alloc();
//     }

//     using propagate_on_container_copy_assignment = std::true_type;

//     template <typename U>
//     struct rebind
//     {
//         using other = new_alloc<U>;
//     };
//     template <class... Args >
//     void construct (T* p, Args&&... args)
//     {
//         ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
//     }
    
//     void destroy (T* p)
//     {
//         p->~T();
//     }
// };

// void map_allocator_test()
// {   
//     using mapa = UnorderedMap < std::string, 
//                                 int,
//                                 std::hash <std::string>,
//                                 std::equal_to <std::string>,
//                                 new_alloc<std::pair<const std::string, int>>
//                                 >;
//     mapa map;
//     new_alloc<int> alloc;

//     //mapa map2 (alloc);

//     map["1"] = 1;
//     map.insert ({"2", 2});
//     map.emplace ("3", 3);
//     assert (map.size() == 3);

//     mapa map3 (map);
//     //assert (! (map.get_allocator() == map3.get_allocator()));

//     map3 ["4"] = 4;
//     map3.erase (map3.find("3"));

//     //map = map3;

//     assert (map.size() == 3);
//     assert (map3.size() == 3);
//     //assert (map.get_allocator() == map3.get_allocator());
//  }