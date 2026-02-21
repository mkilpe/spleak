#include "memory_map.hpp"
#include "logger.hpp"
#include <algorithm>

namespace securepath::spleak {

namespace {

struct tarjan_scc {
	unordered_map<void const*, int> index;
	unordered_map<void const*, int> lowlink;
	set<void const*> on_stack;
	vector<void const*> stack;
	int counter{};
	vector<vector<void const*>> cycles;

	template<typename ForNeighbors>
	void visit(void const* v, ForNeighbors& for_neighbors) {
		index[v] = lowlink[v] = counter++;
		stack.push_back(v);
		on_stack.insert(v);
		for_neighbors(v, [&](void const* w) { process_edge(v, w, for_neighbors); });
		if(lowlink[v] == index[v]) {
			pop_scc(v, for_neighbors);
		}
	}

	template<typename ForNeighbors>
	void process_edge(void const* v, void const* w, ForNeighbors& for_neighbors) {
		if(!index.count(w)) {
			visit(w, for_neighbors);
			lowlink[v] = std::min(lowlink[v], lowlink[w]);
		} else if(on_stack.count(w)) {
			lowlink[v] = std::min(lowlink[v], index[w]);
		}
	}

	template<typename ForNeighbors>
	void pop_scc(void const* v, ForNeighbors& for_neighbors) {
		vector<void const*> scc;
		void const* w;
		do {
			w = stack.back();
			stack.pop_back();
			on_stack.erase(w);
			scc.push_back(w);
		} while(w != v);
		if(scc.size() > 1) {
			if(auto cycle = find_cycle(scc, for_neighbors); !cycle.empty()) {
				cycles.push_back(std::move(cycle));
			}
		}
	}

	template<typename ForNeighbors>
	vector<void const*> find_cycle(vector<void const*> const& scc, ForNeighbors& for_neighbors) {
		set<void const*> scc_set(scc.begin(), scc.end());
		void const* const start = scc[0];
		vector<void const*> path = {start};
		set<void const*> visited = {start};

		auto dfs = [&](auto& self, void const* v) -> bool {
			bool found = false;
			for_neighbors(v, [&](void const* w) {
				if(found || !scc_set.count(w)) {
					return;
				}
				if(w == start) { 
					path.push_back(start);
					found = true;
					return; 
				}
				if(visited.count(w)) {
					return;
				}
				visited.insert(w);
				path.push_back(w);
				if(self(self, w)) { 
					found = true; 
					return; 
				}
				path.pop_back();
			});
			return found;
		};

		return dfs(dfs, start) ? path : vector<void const*> {};
	}

	template<typename Vertices, typename ForNeighbors>
	vector<vector<void const*>> collect_cycles(Vertices const& vertices, ForNeighbors& for_neighbors) {
		for(auto const& [v, _] : vertices) {
			if(!index.count(v)) {
				visit(v, for_neighbors);
			}
		}
		return cycles;
	}
};

} // namespace

bool memory_map::add_alloc(void const* address, std::uint64_t size) {
	auto it = mem_.find(address);
	bool ret = it == mem_.end();
	if(ret) {
		mem_.emplace(address, memory_block{address, size});
	}
	return ret;
}

bool memory_map::remove_alloc(void const* address) {
	auto it = mem_.find(address);
	bool ret = it != mem_.end();
	if(ret) {
		mem_.erase(it);
	}
	return ret;
}

void memory_map::add_owned_memory(owner_memory_block const& mem) {
	ownership_[mem.owner].push_back(ownership_edge{mem.mem, mem.size});
}

void memory_map::remove_owned_memory(void const* owner, void const* containee) {
	auto it = ownership_.find(owner);
	if(it == ownership_.end()) {
		return;
	}

	auto& edges = it->second;
	auto eit = std::find_if(edges.begin(), edges.end(), [&](auto const& e) {
		return e.containee == containee;
	});

	if(eit != edges.end()) {
		edges.erase(eit);
	}

	if(edges.empty()) {
		ownership_.erase(it);
	}
}

void memory_map::move_owned_memory(void const* old_owner, void const* new_owner, void const* containee) {
	auto it = ownership_.find(old_owner);
	if(it == ownership_.end()) {
		return;
	}

	auto& edges = it->second;
	auto eit = std::find_if(edges.begin(), edges.end(), [&](auto const& e) {
		return e.containee == containee;
	});

	if(eit == edges.end()) {
		return;
	}

	ownership_edge edge = *eit;
	edges.erase(eit);
	if(edges.empty()) {
		ownership_.erase(it);
	}
	ownership_[new_owner].push_back(edge);
}

vector<vector<void const*>> memory_map::collect_cycles() const {
	// Visit all owner-graph neighbors of v: direct containees that are themselves
	// owners, plus any other owners embedded within the containee range.
	auto for_neighbors = [&](void const* v, auto&& fn) {
		auto it = ownership_.find(v);
		if(it == ownership_.end()) {
			return;
		}
		for(auto const& edge : it->second) {
			if(ownership_.count(edge.containee)) {
				fn(edge.containee);
			}
			auto const* begin = static_cast<char const*>(edge.containee);
			auto lb = ownership_.lower_bound(edge.containee);
			auto ub = ownership_.lower_bound(static_cast<void const*>(begin + edge.size));
			for(auto oit = lb; oit != ub; ++oit) {
				if(oit->first != edge.containee) {
					fn(oit->first);
				}
			}
		}
	};

	tarjan_scc tarjan;
	return tarjan.collect_cycles(ownership_, for_neighbors);
}

void memory_map::print_cycles(logger& log, std::size_t /*max_depth*/) const {
	if(ownership_.empty()) {
		return;
	}

	auto cycles = collect_cycles();
	if(cycles.empty()) {
		return;
	}

	log.log("detected {} ownership cycle(s):", std::uint64_t(cycles.size()));
	for(auto const& path : cycles) {
		log.log("  cycle:");
		for(std::size_t i = 0; i + 1 < path.size(); ++i) {
			log.log("    {} -> {}", path[i], path[i + 1]);
		}
		log.log("  allocations:");
		for(std::size_t i = 0; i + 1 < path.size(); ++i) {
			auto it = mem_.find(path[i]);
			if(it != mem_.end()) {
				log.log("    {}:", path[i]);
				it->second.strace.print_trace(log);
			}
		}
	}
}

}