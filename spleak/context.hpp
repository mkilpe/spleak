#pragma once

#include "settings.hpp"
#include "statistics.hpp"
#include <core/logger.hpp>
#include <core/static_allocator.hpp>

#include <core/memory_map.hpp>

namespace securepath::spleak {

// singleton context for dynamic/shared library
class context {
public:
	static context& instance();

	void add_alloc_mem(void const* address, std::uint64_t size);
	void remove_alloc_mem(void const* address);

	void add_pointer_owner(const void* owner_address, const void* containee_address, std::uint32_t size);
	void remove_pointer_owner(const void* owner_address, const void* containee_address);
	void move_pointer_owner(const void* old_owner, const void* new_owner, const void* containee_address);

	void report_on_shutdown();

	template<typename... Args>
	void log(std::string_view format, Args const&... args) const {
		log_.log(format, args...);
	}

	static_alloc<4096>& static_mem() { return alloc_; }

private:
	mutable mutex mutex_;
	// allocator for logging
	static_alloc<4096> alloc_;
	mutable console_logger log_{alloc_};
	settings settings_;
	memory_map map_;
	statistics stats_;
};

}