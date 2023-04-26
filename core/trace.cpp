#include "trace.hpp"

#include <common/logger.hpp>

#include <execinfo.h>

namespace securepath::spleak {

trace::trace()
{
	size_ = backtrace(trace_, trace_depth);
}

void trace::print_trace() const {
	if(char** strings = backtrace_symbols(trace_, size_)) {
		print("Obtained {} stack frames.", size_);
		for(std::size_t i = 0; i != size_; ++i) {
			print(strings[i]);
		}
		free(strings);
	}
}

}