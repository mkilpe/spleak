#pragma once

#include "allocator.hpp"

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// declare std types using our allocator

namespace securepath::spleak {

using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;

template<typename Key,
		typename Value,
		typename Compare = std::less<Key>>
using map = std::map<Key, Value, Compare, allocator<std::pair<const Key, Value>>>;

template<typename Key,
		typename Value,
		typename Hash = std::hash<Key>,
    	typename KeyEqual = std::equal_to<Key>>
using unordered_map = std::unordered_map<Key, Value, Hash, KeyEqual, allocator<std::pair<const Key, Value>>>;

template<typename Type>
using vector = std::vector<Type, allocator<Type>>;

template<typename Key, typename Compare = std::less<Key>>
using set = std::set<Key, Compare, allocator<Key>>;

using mutex = std::mutex;

template<typename T>
using unique_lock = std::unique_lock<T>;

}