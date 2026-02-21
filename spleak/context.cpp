
#include "context.hpp"

namespace securepath::spleak {

context& context::instance() {
	static context instance;
	return instance;
}

void context::add_alloc_mem(void const* address, std::uint64_t size) {
	unique_lock lock{mutex_};
	map_.add_alloc(address, size);
	if(settings_.collect_memory_allocation_statistics) {
		stats_.add_allocation(alloc_info{size});
	}
}

void context::remove_alloc_mem(void const* address) {
	unique_lock lock{mutex_};
	map_.remove_alloc(address);
	if(settings_.collect_memory_allocation_statistics) {
		stats_.add_deallocation(dealloc_info{});
	}
}

void context::add_pointer_owner(void const* owner_address, void const* containee_address, std::uint32_t size) {
	unique_lock lock{mutex_};
	map_.add_owned_memory(owner_memory_block{owner_address, containee_address, size});
}

void context::remove_pointer_owner(void const* owner_address, void const* containee_address) {
	unique_lock lock{mutex_};
	map_.remove_owned_memory(owner_address, containee_address);
}

void context::move_pointer_owner(void const* old_owner, void const* new_owner, void const* containee_address) {
	unique_lock lock{mutex_};
	map_.move_owned_memory(old_owner, new_owner, containee_address);
}

void context::report_on_shutdown() {
	unique_lock lock{mutex_};
	for(auto&& m : map_) {
        log_.log("unreleased memory: {} size={}", m.second.address, m.second.size);
        m.second.strace.print_trace(log_);
    }
    if(settings_.analyse_memory_owner_cycles) {
        map_.print_cycles(log_, settings_.owner_cycle_depth);
    }
    if(settings_.collect_memory_allocation_statistics) {
        stats_.print(log_);
    }
}

}