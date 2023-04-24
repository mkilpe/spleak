
#include "config.hpp"
#include <common/logger.hpp>
#include <core/memory_map.hpp>

static securepath::spleak::memory_map& memmap() {
	static securepath::spleak::memory_map map;
	return map;
}

#ifdef _WIN32
//#ifdef _MSC_VER

#include <malloc.h>

#ifdef _DEBUG
#include <crtdbg.h>
#else
#include <windows.h>
#endif

extern "C" {

SPLEAK_EXPORT void* malloc(size_t size) {
    print("malloc");
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

}

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <unistd.h>

template<typename Func>
static Func* resolve(char const* name) {
	return (Func*)dlsym(RTLD_NEXT, name);
}

extern "C" {

SPLEAK_EXPORT void* malloc(size_t size) {
    auto r = resolve<decltype(malloc)>("malloc")(size);
    securepath::print("malloc {}", r);
    memmap().add(r, size);
	return r;
}

SPLEAK_EXPORT void* calloc(size_t size, size_t n) {
    securepath::print("calloc");
	return resolve<decltype(calloc)>("calloc")(size, n);
}

SPLEAK_EXPORT void* realloc(void* p, size_t newsize) {
    securepath::print("realloc");
	return resolve<decltype(realloc)>("realloc")(p, newsize);
}

SPLEAK_EXPORT void free(void* p) {
    securepath::print("free {}", p);
    memmap().remove(p);
	return resolve<decltype(free)>("free")(p);
}

SPLEAK_EXPORT void* sp_real_alloc(std::size_t size) {
    return resolve<decltype(malloc)>("malloc")(size);
}

SPLEAK_EXPORT void sp_real_dealloc(void* p, std::size_t) {
    resolve<decltype(free)>("free")(p);
}

void __attribute__ ((constructor)) spleak_init() {

}

void __attribute__ ((destructor)) spleak_fini() {
    for(auto&& m : memmap()) {
        securepath::print("unreleased memory: {} size={}", m.second.address, m.second.size);
    }
}

}

#endif