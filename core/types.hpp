#pragma once

#include "allocator.hpp"

#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// declare std types using our allocator

namespace securepath::spleak {

using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;

template<typename Key,
		typename Value,
		typename Hash = std::hash<Key>,
    	typename KeyEqual = std::equal_to<Key>>
using unordered_map = std::unordered_map<Key, Value, Hash, KeyEqual, allocator<std::pair<const Key, Value>>>;

template<typename Type>
using vector = std::vector<Type, allocator<Type>>;

using mutex = std::mutex;

template<typename T>
using unique_lock = std::unique_lock<T>;

}