
#include "config.hpp"

#ifdef _MSC_VER

#ifdef _DEBUG
#include <crt.h>
#else
#include <crtdbg.h>
#endif

extern "C" {

SPLEAK_EXPORT void* malloc(size_t size) {
#ifdef _DEBUG
    return _malloc_dbg(size, _NORMAL_BLOCK, nullptr, 0);
#else
    return _malloc_base(size);
#endif
}

SPLEAK_EXPORT void* calloc(size_t num, size_t size) {
#ifdef _DEBUG
    return _calloc_dbg(num, size, _NORMAL_BLOCK, nullptr, 0);
#else
    return _calloc_base(num, size);
#endif
}

SPLEAK_EXPORT void* realloc(void* p, size_t newsize) {
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

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>

template<typename Func>
static Func* resolve(char const* name) {
	return (Func*)dlsym(RTLD_NEXT, name);
}

extern "C" {

SPLEAK_EXPORT void* malloc(size_t size) {
	return resolve<decltype(malloc)>("malloc")(size);
}

SPLEAK_EXPORT void* calloc(size_t size, size_t n) {
	return resolve<decltype(malloc)>("calloc")(n);
}

SPLEAK_EXPORT void* realloc(void* p, size_t newsize) {
	return resolve<decltype(realloc)>("realloc")(p, newsize);
}

SPLEAK_EXPORT void free(void* p) {
	return resolve<decltype(free)>("free")(p);
}

}

#endif