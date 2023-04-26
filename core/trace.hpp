#pragma once

#include "types.hpp"

namespace securepath::spleak {

class trace {
public:
	static constexpr std::size_t trace_depth = 50;

	trace();
	void print_trace() const;

private:
	void* trace_[trace_depth];
	std::size_t size_ {};
};

}
