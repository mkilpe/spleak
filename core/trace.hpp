#pragma once

#include "logger.hpp"
#include "types.hpp"

namespace securepath::spleak {

class trace {
public:
	static constexpr std::size_t trace_depth = 1024;

	trace();
	void print_trace(logger&) const;

private:
	void* trace_[trace_depth];
	std::size_t size_ {};
};

}
