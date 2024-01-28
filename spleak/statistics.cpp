#include "statistics.hpp"

namespace securepath::spleak {

void statistics::add_allocation(const alloc_info& info) {
	++num_allocs_;
	current_alloc_size_ += info.size;
	sizes_.max = std::max(sizes_.max, info.size);
	sizes_.total += info.size;
	sizes_.peak = std::max(sizes_.max, current_alloc_size_);
	//todo: no time or speed info implemented yet
}

void statistics::add_deallocation(const dealloc_info& info) {
	current_alloc_size_ -= info.size;
}

void statistics::print(logger& l) const {
	l.log("-- Statistics --");
	l.log("total unreleased memory: {}", current_alloc_size_);
	l.log("number of allocations: {}", num_allocs_);
	l.log("max single alloc: {}", sizes_.max);
	l.log("total alloc: {}", sizes_.total);
	l.log("most allocated at one time: {}", sizes_.peak);
}

}