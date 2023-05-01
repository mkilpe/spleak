#pragma once

#include "settings.hpp"
#include <core/logger.hpp>

#include <core/memory_map.hpp>

namespace securepath::spleak {

// singleton context for dynamic/shared library
class context {
public:
	static context& instance();

	void add_alloc_mem(void const* address, std::uint64_t size);
	void remove_alloc_mem(void const* address);

	void report_on_shutdown();

private:
	mutable mutex mutex_;
	console_logger log_;
	settings settings_;
	memory_map map_;
};

}