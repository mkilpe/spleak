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
	// how long the allocation was alive
	std::chrono::nanoseconds lifetime;
};

struct size_bucket {
	std::size_t const max_size;
	std::size_t count;
};

class statistics {
public:
	void add_allocation(alloc_info);
	void add_dealloction(dealloc_info);

	void print(logger&) const;
private:
	std::size_t num_allocs_{};
	struct size_info {
		std::size_t max{};
		std::size_t total{};
		std::size_t count{};
	} sizes_;
	struct time_info {
		std::size_t min{};
		std::size_t max{};
		std::size_t total{};
		std::size_t count{};
	} times_;
	struct speed_info {
		std::size_t min{};
		std::size_t max{};
		std::size_t total{};
		std::size_t count{};
	} speeds_;
	vector<size_bucket> size_buckets_;
};

}