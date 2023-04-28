
#include "alloc.hpp"
#include "config.hpp"
#include "context.hpp"

#include <common/logger.hpp>

#ifdef _WIN32
//#ifdef _MSC_VER

#include <malloc.h>

#ifdef _DEBUG
#include <crtdbg.h>
#else
#include <windows.h>
#endif

namespace securepath::spleak {

extern "C" {

SPLEAK_EXPORT void* malloc(std::size_t size) {
    print("malloc");
#ifdef _DEBUG
    return _malloc_dbg(size, _NORMAL_BLOCK, nullptr, 0);
#else
    return _malloc_base(size);
#endif
}

SPLEAK_EXPORT void* calloc(std::size_t num, std::size_t size) {
#ifdef _DEBUG
    return _calloc_dbg(num, size, _NORMAL_BLOCK, nullptr, 0);
#else
    return _calloc_base(num, size);
#endif
}

SPLEAK_EXPORT void* realloc(void* p, std::size_t newsize) {
#ifdef _DEBUG
    return _realloc_dbg(p, newsize, _NORMAL_BLOCK, nullptr, 0);
#else
    return _realloc_base(p, newsize);
#endif
}

SPLEAK_EXPORT void free(void* p) {
#ifdef _DEBUG
    return _free_dbg(p, _NORMAL_BLOCK);
#else
    return _free_base(p);
#endif
}

}
}
