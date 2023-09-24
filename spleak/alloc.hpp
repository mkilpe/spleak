#pragma once

#include <core/config.hpp>
#include <cstddef>

namespace securepath::spleak {

bool start_internal_op();
void end_internal_op();

struct scoped_internal_op {
	scoped_internal_op() : started(start_internal_op()) {}
	~scoped_internal_op() { if(started) { end_internal_op(); } }

	explicit operator bool() const {
		return started;
	}

	bool started{};

	scoped_internal_op(scoped_internal_op const&) = delete;
	scoped_internal_op& operator=(scoped_internal_op const&) = delete;
};

void* malloc_op(void* r, std::size_t size);
void* calloc_op(void* r, std::size_t size, std::size_t n);
void* realloc_op(void* r, void* p, std::size_t newsize);
void  free_op(void* p);

}
