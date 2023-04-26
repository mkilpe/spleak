#pragma once

#include "types.hpp"
#include "trace.hpp"
#include <thread>

namespace securepath::spleak {

class memory_map {
public:
	bool add(void const* address, std::uint64_t size);
	bool remove(void const* address);

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