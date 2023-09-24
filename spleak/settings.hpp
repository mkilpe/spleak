#pragma once

#include <cstddef>

namespace securepath::spleak {

struct settings {
	// Should we save stack trace, this is needed if wanting to see stack trace
	// for leaked memory.
	bool save_trace_to_allocation{true};
	// Try to resolve module handle for allocations/frees and see if those match.
	bool resolve_module{true};
	// Collect general statistics for memory allocations.
	bool collect_memory_allocation_statistics{false};
	// Try to find memory allocation patterns.
	bool analyse_allocation_patterns{false};
	// Try to find memory owner cycles
	bool analyse_memory_owner_cycles{false};
	// Depth to analyse cycles
	std::size_t owner_cycle_depth{8};
};

}