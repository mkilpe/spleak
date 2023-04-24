#pragma once

#include "types.hpp"

namespace securepath::spleak {

class memory_map {
public:
	bool add(void const* address, std::size_t size);
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
		std::size_t size{};
	};
private:
	mutable mutex mutex_;
	unordered_map<void const*, memory_block> mem_;
};

}