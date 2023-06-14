#pragma once

#include "types.hpp"
#include "trace.hpp"
#include <thread>

namespace securepath::spleak {

struct owner_memory_block {
	const void* owner;
	const void* mem;
	std::uint32_t size;
};

class memory_map {
public:
	bool add_alloc(void const* address, std::uint64_t size);
	bool remove_alloc(void const* address);

	bool add_owned_memory(const owner_memory_block& mem);
	//bool remove_owned_memory(const void* )

	auto begin() const {
		return mem_.begin();
	}

	auto end() const {
		return mem_.end();
	}

private:
	struct memory_block {
		void const* address;
		std::uint64_t size{};
		std::thread::id thread = std::this_thread::get_id();
		trace strace;
	};
private:
	unordered_map<void const*, memory_block> mem_;
};

}