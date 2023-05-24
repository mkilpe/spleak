
#ifdef _WIN32
#include "alloc_win.hpp"
#else
#include "alloc_linux.hpp"
#endif

namespace securepath::spleak {

std::atomic_flag& iop() {
    thread_local std::atomic_flag internal_op{};
    return internal_op;
}

bool start_internal_op() {
    return !iop().test_and_set();
}

void end_internal_op() {
    iop().clear();
}

void* malloc_op(void* r, std::size_t size) {
    if(scoped_internal_op _{}) {
        context::instance().add_alloc_mem(r, size);
    }
    return r;
}

void* calloc_op(void* r, std::size_t size, std::size_t n) {
    if(scoped_internal_op _{}) {
    }
    return r;
}

void* realloc_op(void* r, void* p, std::size_t newsize) {
    if(scoped_internal_op _{}) {
        context::instance().remove_alloc_mem(p);
        context::instance().add_alloc_mem(r, newsize);
    }
    return r;
}

void free_op(void* p) {
    if(scoped_internal_op _{}) {
        context::instance().remove_alloc_mem(p);
    }
}

}