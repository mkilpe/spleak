
#include "context.hpp"

namespace securepath::spleak {

context& context::instance() {
	static context instance;
	return instance;
}

void context::add_alloc_mem(void const* address, std::uint64_t size) {
	unique_lock lock{mutex_};
	map_.add(address, size);
}

void context::remove_alloc_mem(void const* address) {
	unique_lock lock{mutex_};
	map_.remove(address);
}

void context::report_on_shutdown() {
	unique_lock lock{mutex_};
	for(auto&& m : map_) {
        log_.log("unreleased memory: {} size={}", m.second.address, m.second.size);
        m.second.strace.print_trace(log_);
    }
}

}