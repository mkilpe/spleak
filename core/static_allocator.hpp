#pragma once

#include <memory>

namespace securepath::spleak {

// simple static memory allocator that only supports freeing the last allocation
// used for dll entry allocation and logging, so can be simple like this
template<std::size_t Size>
class static_alloc {
public:
	//do aligned alloc
	void* alloc(std::size_t size, std::size_t alignment) {
		void* addr = first_unused_;
		std::size_t free_space = (mem_+Size) - first_unused_;
		if(!std::align(alignment, size, addr, free_space)) {
			throw std::bad_alloc();
        }
        first_unused_ += size;
        last_alloc_address_ = addr;
        return addr;
	}

	template<typename T>
	void* alloc(std::size_t size, std::size_t alignment = alignof(T)) {
		return alloc(size*sizeof(T), alignment);
	}

	void dealloc(void* p, std::size_t size) {
		// we only support deallocating the last allocated block
		if(p == last_alloc_address_) {
			first_unused_ = static_cast<char*>(last_alloc_address_);
		}
	}

	bool in_range(void* p) const {
		return mem_ <= p && p < mem_+Size;
	}
private:
	// use somewhat useful alignment as default
	alignas(16) char mem_[Size]{};
	char* first_unused_{mem_};
	void* last_alloc_address_{};
};

template <class T, class StaticAllocator>
struct static_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    constexpr static_allocator() noexcept = default;
    constexpr static_allocator(static_allocator const&) noexcept = default;
    constexpr static_allocator(StaticAllocator& alloc) : alloc_(&alloc) {}

    template <class U>
    constexpr static_allocator(static_allocator<U, StaticAllocator> const& a) noexcept
    : alloc_(a.alloc_)
    {}

	[[nodiscard]] T* allocate(std::size_t n) {
		return reinterpret_cast<T*>(alloc_->template alloc<T>(n));
	}

    void deallocate(T* p, size_type s) {
    	alloc_->dealloc(p, s);
    }

private:
	StaticAllocator* alloc_{};
};

}
