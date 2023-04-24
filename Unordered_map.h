#include <iterator>
#include <vector>
#include <cstddef>
#include <cmath>

template <typename T, typename Allocator = std::allocator<T>>
class List {
    
};


template <typename Key, typename Value, typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
    static const size_t DEFAULT_CAPACITY = 5;
    constexpr static const float DEFAULT_LOAD_FACTOR = 0.95;
    constexpr static const float DEFAULT_EXPAND_FACTOR = 2;

    
};