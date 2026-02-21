# Spleak

A memory leak detection and analysis tool for C++ applications. Spleak hooks memory allocation functions at runtime to track allocations, detect leaks, report stack traces, and identify ownership cycles.

## Features

- **Leak detection**: Track all heap allocations and report unreleased memory at shutdown
- **Stack traces**: Capture and display where each allocation occurred
- **Ownership cycle detection**: Identify cyclic ownership relationships
- **Memory statistics**: Report allocation counts, sizes, and peak usage
- **Cross-platform**: Linux (shared object preload) and Windows (DLL injection)

## Building

**Requirements:** CMake 3.12+, C++20 compiler

### Linux

```bash
cmake -B build
cmake --build build
```

Outputs:
- `bin/libspleak.so` — preloadable shared library
- `bin/test_app` — basic allocation test
- `bin/test_owner_cycles` — cycle detection test

### Windows (cross-compile from Linux with MinGW)

```bash
mingw64-cmake -B build-win64
cmake --build build-win64
```

See `doc/cross-compile.txt` for details on testing with Wine.

## Usage

### Linux

Preload the shared library before your application:

```bash
LD_PRELOAD=/path/to/libspleak.so ./your_application
```

At exit, Spleak prints a report of any leaked allocations and detected ownership cycles.

### Ownership tracking (optional)

Include `spleak/spleak.hpp` and use the C API to register ownership relationships:

```cpp
#include <spleak/spleak.hpp>

// Register that `owner` owns the memory at `ptr`
spleak_add_pointer_owner(ptr, owner);

// Remove ownership
spleak_remove_pointer_owner(ptr, owner);

// Transfer ownership
spleak_move_pointer_owner(ptr, old_owner, new_owner);
```

This enables Spleak to detect reference cycles that would prevent memory from being freed.

## Configuration

Settings are in `spleak/settings.hpp`:

| Setting | Default | Description |
|---|---|---|
| `save_trace_to_allocation` | `true` | Capture stack trace per allocation |
| `resolve_module` | `true` | Verify allocations freed in same module |
| `collect_memory_allocation_statistics` | `true` | Track size/count statistics |
| `analyse_memory_owner_cycles` | `true` | Run cycle detection at shutdown |
| `owner_cycle_depth` | `8` | Max depth for cycle search |

## How it works

Spleak interposes the standard allocation functions (`malloc`, `calloc`, `realloc`, `free`) using `LD_PRELOAD` on Linux and DLL injection on Windows. Each allocation is recorded with its address, size, thread ID, and optional stack trace.

At shutdown, Spleak walks the recorded allocations, reports any that were not freed, and (if enabled) analyzes the ownership graph for cycles using Tarjan's strongly connected components algorithm.

Stack traces use `backtrace()`/`backtrace_symbols()` on Linux and `RtlCaptureStackBackTrace`/`SymFromAddr` on Windows, with C++ symbol demangling.
