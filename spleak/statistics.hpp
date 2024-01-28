#pragma once

#include <core/logger.hpp>

#include <cstdint>
#include <chrono>

namespace securepath::spleak {

struct alloc_info {
	// size of the allocation
	std::size_t size;
	//how long the allocation took
	std::chrono::nanoseconds speed;
};

struct dealloc_info {
	// size of the allocation
	std::size_t size;
	// how long the allocation was alive
	std::chrono::nanoseconds lifetime;
};

struct size_bucket {
	std::size_t const max_size;
	std::size_t count;
};

class statistics {
public:
	void add_allocation(const alloc_info&);
	void add_deallocation(const dealloc_info&);

	void print(logger&) const;
private:
	std::size_t num_allocs_{};
	std::size_t current_alloc_size_{};
	struct size_info {
		std::size_t max{};
		std::size_t total{};
		// maximum total size allocated at the same time
		std::size_t peak{};
	} sizes_;
	struct time_info {
		std::size_t min{};
		std::size_t max{};
		std::size_t total{};
	} times_;
	struct speed_info {
		std::size_t min{};
		std::size_t max{};
		std::size_t total{};
	} speeds_;
	vector<size_bucket> size_buckets_;
};

}