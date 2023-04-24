
#include "logger.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace securepath {

void print(std::string_view str) {
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

}
