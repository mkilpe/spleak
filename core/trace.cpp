#include "trace.hpp"

#ifdef _WIN32
#else
#include <execinfo.h>
#endif

namespace securepath::spleak {

#ifdef _WIN32
trace::trace()
{
}

void trace::print_trace(logger& l) const {
}

#else

trace::trace()
{
	size_ = backtrace(trace_, trace_depth);
}

void trace::print_trace(logger& l) const {
	if(char** strings = backtrace_symbols(trace_, size_)) {
		l.log("Obtained {} stack frames.", size_);
		for(std::size_t i = 0; i != size_; ++i) {
			l.log(strings[i]);
		}
		free(strings);
	}
}
#endif
}