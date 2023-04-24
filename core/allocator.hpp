#pragma once

#include <cstddef>

extern "C" {
void* sp_real_alloc(std::size_t);
void* sp_real_dealloc(void*, std::size_t);
}

namespace securepath {

template <class T>
struct allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    constexpr allocator() noexcept = default;
    constexpr allocator(allocator const&) noexcept = default;

    template <class U>
    constexpr allocator(allocator<U> const&) noexcept
    {}

	[[nodiscard]] T* allocate(std::size_t n) {
		auto p = (T*)sp_real_alloc(s * sizeof(T));
        if(!p) {
            throw std::bad_alloc();
        }
        return p;
	}

    void deallocate(T* p, size_type s) {
        sp_real_dealloc(p, s);
    }
};

}
