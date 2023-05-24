
#include "alloc.hpp"
#include "config.hpp"
#include "context.hpp"

#include <core/logger.hpp>

#include <atomic>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <unistd.h>

namespace securepath::spleak {

template<typename Func>
static Func* resolve(char const* name) {
	return (Func*)dlsym(RTLD_NEXT, name);
}

extern "C" {

SPLEAK_EXPORT void* sp_real_alloc(std::size_t size) {
	static auto malloc_fun = resolve<decltype(malloc)>("malloc");
	return malloc_fun(size);
}

SPLEAK_EXPORT void sp_real_dealloc(void* p, std::size_t) {
	static auto free_fun = resolve<decltype(free)>("free");
	free_fun(p);
}

SPLEAK_EXPORT void* malloc(std::size_t size) {
	static auto malloc_fun = resolve<decltype(malloc)>("malloc");
	return malloc_op(malloc_fun(size), size);
}

SPLEAK_EXPORT void* calloc(std::size_t size, std::size_t n) {
	static auto calloc_fun = resolve<decltype(calloc)>("calloc");
	return calloc_op(calloc_fun(size, n), size, n);
}

SPLEAK_EXPORT void* realloc(void* p, std::size_t newsize) {
	static auto realloc_func = resolve<decltype(realloc)>("realloc");
	return realloc_op(realloc_func(p, newsize), p, newsize);
}

SPLEAK_EXPORT void free(void* p) {
	static auto free_fun = resolve<decltype(free)>("free");
	free_op(p);
	free_fun(p);
}

void __attribute__ ((constructor)) spleak_init() {
}

void __attribute__ ((destructor)) spleak_fini() {
	if(scoped_internal_op _{}) {
		context::instance().report_on_shutdown();
	}
}

}
}
