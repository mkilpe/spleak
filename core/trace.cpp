#include "trace.hpp"

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <string>
#include <string_view>

namespace securepath::spleak {

#ifdef _WIN32

trace::trace()
{
	//should we support StackWalk64 at some point too?
	size_ = RtlCaptureStackBackTrace(0, trace_depth, trace_, nullptr);
}

static bool is_spleak_symbol(char const* name) {
	return std::string_view{name}.find("securepath::spleak::") != std::string_view::npos;
}

void trace::print_trace(logger& l) const {
	HANDLE process = GetCurrentProcess();
	[[maybe_unused]] static BOOL init = SymInitialize(process, NULL, TRUE);

	constexpr std::size_t max_name_size = 2048;
	alignas(SYMBOL_INFO) char mem[sizeof(SYMBOL_INFO)+max_name_size]{};
	SYMBOL_INFO* symbol = new (mem) SYMBOL_INFO;

	symbol->MaxNameLen = max_name_size;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for(size_t i = 0; i != size_; ++i) {
		if(SymFromAddr(process, (DWORD64)trace_[i], 0, symbol)) {
			if(!is_spleak_symbol(symbol->Name)) {
				l.log(symbol->Name);
			}
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

// backtrace_symbols format: "module(mangled+offset) [addr]"
static bool is_spleak_frame(char const* symbol) {
	std::string_view sv{symbol};
	auto open = sv.find('(');
	if(open == std::string_view::npos) {
		return false; 
	}
	auto end = sv.find('+', open + 1);
	if(end == std::string_view::npos) {
		end = sv.find(')', open + 1);
	}
	if(end == std::string_view::npos || end == open + 1) { 
		return false; 
	}

	std::string mangled{sv.substr(open + 1, end - open - 1)};
	int status;
	char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
	if(!demangled) { 
		return false; 
	}
	bool result = std::string_view{demangled}.starts_with("securepath::spleak::");
	free(demangled);
	return result;
}

void trace::print_trace(logger& l) const {
	if(char** strings = backtrace_symbols(trace_, size_)) {
		for(std::size_t i = 0; i != size_; ++i) {
			if(!is_spleak_frame(strings[i])) {
				l.log(strings[i]);
			}
		}
		free(strings);
	}
}

#endif

}
