#pragma once

#include "types.hpp"
#include "trace.hpp"
#include <thread>

namespace securepath::spleak {

struct owner_memory_block {
	void const* owner;
	void const* mem;
	std::uint32_t size;
};

class logger;

class memory_map {
public:
	bool add_alloc(void const* address, std::uint64_t size);
	bool remove_alloc(void const* address);

	void add_owned_memory(owner_memory_block const& mem);
	void remove_owned_memory(void const* owner, void const* containee);
	void move_owned_memory(void const* old_owner, void const* new_owner, void const* containee);

	auto begin() const {
		return mem_.begin();
	}
	auto end() const {
		return mem_.end();
	}

	void print_cycles(logger& log, std::size_t max_depth) const;
private:
	struct memory_block {
		void const* address;
		std::uint64_t size{};
		std::thread::id thread = std::this_thread::get_id();
		trace strace;
	};

	struct ownership_edge {
		void const* containee;
		std::uint32_t size;
	};
	
	vector<vector<void const*>> collect_cycles() const;

private:
	unordered_map<void const*, memory_block> mem_;
	// ownership graph: owner -> list of (containee, size), sorted by address for range queries
	map<void const*, vector<ownership_edge>> ownership_;
};

}