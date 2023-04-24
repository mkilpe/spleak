
#include "logger.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace securepath {

void print_impl(std::string_view str) {
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(handle != INVALID_HANDLE_VALUE) {
        DWORD count = 0;
        WriteConsole(handle, str, str.size(), &count, NULL);
    }
#else
	write(1, str.data(), str.size());
#endif
}

void print_impl(void const* p) {
	char const hex[] = "0123456789abcdef";
	static_assert(sizeof(p) == 8);
	char arr[18] = {};
	std::uintptr_t n = reinterpret_cast<std::uintptr_t>(p);
	int i = 0;
	do {
		arr[17-i] = hex[(n & 0xF)];
		n >>= 4;
		++i;
	} while(i != 16 && n);
	arr[18-i-1]	= 'x';
	arr[18-i-2]	= '0';
	print_impl(std::string_view(arr+18-i-2, arr+18));
}

void print_impl(std::uint64_t v) {
	char arr[19] = {};
	int i = 0;
	do {
		arr[18-i] = '0' + (v % 10);
		v /= 10;
		++i;
	} while(i != 19 && v);
	print_impl(std::string_view(arr+19-i, arr+19));
}

}
