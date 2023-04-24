#include "memory_map.hpp"

namespace securepath::spleak {

bool memory_map::add(void const* address, std::size_t size) {
	unique_lock lock{mutex_};
	auto it = mem_.find(address);
	bool ret = it == mem_.end();
	if(ret) {
		mem_.emplace(address, memory_block{address, size});
	}
	return ret;
}

bool memory_map::remove(void const* address) {
	unique_lock lock{mutex_};
	auto it = mem_.find(address);
	bool ret = it != mem_.end();
	if(ret) {
		mem_.erase(it);
	}
	return ret;
}

}