#pragma once

#if JE_DATA_STRUCTS_STD_BACKEND

#include <functional>
#include <vector>
#include <stack>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

#include "mem/std_wrapper.h"

namespace je { namespace data {

    template <typename object_type> using allocator_type = mem::std_wrapper<object_type>;

    template <typename object_type> using sort_less = std::less<object_type>;
    template <typename object_type> using sort_greater = std::greater<object_type>;
    template <typename object_type> using hash = std::hash<object_type>;
    template <typename object_type> using key_equal = std::equal_to<object_type>;

    template <typename object_type> using array = std::vector<object_type, allocator_type<object_type>>;
    using bit_array = std::vector<bool, allocator_type<bool>>;
    template <typename object_type> using stack = std::stack<object_type, allocator_type<object_type>>;
    template <typename object_type> using queue = std::deque<object_type, allocator_type<object_type>>;
    template <typename object_type, class hash_type = hash<object_type>, class key_cmp_type = key_equal<object_type>> using set = std::unordered_set<object_type, hash_type, key_cmp_type, allocator_type<object_type>>;
    template <typename key_type, typename object_type, class hash_type = hash<object_type>, class key_cmp_type = key_equal<object_type>> using dict = std::unordered_map<key_type, object_type, hash_type, key_cmp_type, allocator_type<object_type>>;
    template <typename object_type, class sort_type = sort_less<object_type>> using sorted_set = std::set<object_type, sort_type, allocator_type<object_type>>;
    template <typename key_type, typename object_type, class sort_type = sort_less<object_type>> using sorted_map = std::map<key_type, object_type, sort_type, allocator_type<object_type>>;

}}
#else

#error "Not implemented custom data structs backend."

#endif

#include "static_array.h"
#include "static_bit_array.h"
#include "string.h"