#include "memory_map.hpp"

namespace securepath::spleak {

bool memory_map::add(void const* address, std::uint64_t size) {
	auto it = mem_.find(address);
	bool ret = it == mem_.end();
	if(ret) {
		mem_.emplace(address, memory_block{address, size});
	}
	return ret;
}

bool memory_map::remove(void const* address) {
	auto it = mem_.find(address);
	bool ret = it != mem_.end();
	if(ret) {
		mem_.erase(it);
	}
	return ret;
}

}