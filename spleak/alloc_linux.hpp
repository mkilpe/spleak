
#include "config.hpp"
#include "context.hpp"

#include <common/logger.hpp>

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

thread_local std::atomic_flag internal_op{};

extern "C" {

SPLEAK_EXPORT void* malloc(size_t size) {
	static auto malloc_fun = resolve<decltype(malloc)>("malloc");
	auto r = malloc_fun(size);
	if(!internal_op.test_and_set()) {
		context::instance().add_alloc_mem(r, size);
		internal_op.clear();
	}
	return r;
}

SPLEAK_EXPORT void* calloc(size_t size, size_t n) {
	static auto calloc_fun = resolve<decltype(calloc)>("calloc");
	auto r = calloc_fun(size, n);
	if(!internal_op.test_and_set()) {
		//todo
		internal_op.clear();
	}
	return r;
}

SPLEAK_EXPORT void* realloc(void* p, size_t newsize) {
	static auto realloc_func = resolve<decltype(realloc)>("realloc");
	auto r = realloc_func(p, newsize);
	if(!internal_op.test_and_set()) {
		//todo
		internal_op.clear();
	}
	return r;
}

SPLEAK_EXPORT void free(void* p) {
	static auto free_fun = resolve<decltype(free)>("free");
	free_fun(p);
	if(!internal_op.test_and_set()) {
		context::instance().remove_alloc_mem(p);
		internal_op.clear();
	}
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
	if(!internal_op.test_and_set()) {
		context::instance().report_on_shutdown();
		internal_op.clear();
	}
}

}
}
