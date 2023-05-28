#include "trace.hpp"

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#else
#include <execinfo.h>
#endif

#include <cstring>

namespace securepath::spleak {

#ifdef _WIN32
trace::trace()
{
	//should we support StackWalk64 at some point too?
	size_ = RtlCaptureStackBackTrace(0, trace_depth, trace_, nullptr);
}

void trace::print_trace(logger& l) const {
	HANDLE process = GetCurrentProcess();
	[[maybe_unused]] static BOOL init = SymInitialize(process, NULL, TRUE);

	constexpr std::size_t max_name_size = 2048;
	alignas(SYMBOL_INFO) char mem[sizeof(SYMBOL_INFO)+max_name_size]{};
	SYMBOL_INFO* symbol = new (mem) SYMBOL_INFO;

	symbol->MaxNameLen = max_name_size;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	l.log("Obtained {} stack frames.", size_);
	for(size_t i = 0; i != size_; ++i) {
		if(SymFromAddr(process, (DWORD64)trace_[i], 0, symbol)) {
			l.log(symbol->Name);
		} else {
			l.log("cannot resolve symbol, address = {}", trace_[i]);
		}
	}
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