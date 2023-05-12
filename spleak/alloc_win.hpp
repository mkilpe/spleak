
#include "alloc.hpp"
#include "config.hpp"
#include "context.hpp"

//#ifdef _MSC_VER

#include <windows.h>
#include <psapi.h>

namespace securepath::spleak {
namespace {
    struct resolved_functions {
        void* (*malloc)(std::size_t size);
        void* (*calloc)(std::size_t num, std::size_t size);
        void* (*realloc)(void* p, std::size_t newsize);
        void  (*free)(void* p);
    };

    resolved_functions& impl() {
        static resolved_functions r{};
        return r;
    }

    resolved_functions const& resolved() {
        return impl();
    }

    template<typename Func>
    bool resolve_func(HMODULE m, Func& f, char const* name) {
        f = (Func)GetProcAddress(m, name);
        return f != nullptr;
    }

    bool resolve_functions(HMODULE m, resolved_functions& table) {
        return resolve_func(m, table.malloc, "malloc")
            && resolve_func(m, table.calloc, "calloc")
            && resolve_func(m, table.realloc, "realloc")
            && resolve_func(m, table.free, "free");
    }

    HMODULE find_module() {
        HMODULE mods[4*1024];
        HANDLE process = GetCurrentProcess();
        DWORD mod_handle_bytes{};
        char name[16*1024]; //what is correct size?

        if(EnumProcessModules(process, mods, sizeof(mods), &mod_handle_bytes)) {
            for(std::size_t i = 0; i < (mod_handle_bytes / sizeof(HMODULE)); ++i) {
                if(GetModuleFileNameA(mods[i], name, sizeof(name))) {
                    std::string s(name);
                    //MSVCRXXX.dll
                    if(s.substr(s.find_last_of('\\')) == "msvcrt.dll") { //other names?
                        return mods[i];
                    }

                    context::instance().log("{}", s.substr(s.find_last_of('\\')));
                }
            }
        }
        return nullptr;
    }

    bool resolve_function_table() {
        HMODULE m = find_module();
        // perhaps later on try other strategies if fails to find the module based on dll name?
        return m && resolve_functions(m, impl());
    }

  /*
GetModuleInformation

FARPROC GetProcAddress(
  [in] HMODULE hModule,
  [in] LPCSTR  lpProcName
);

DWORD GetModuleBaseNameA(
  [in]           HANDLE  hProcess,
  [in, optional] HMODULE hModule,
  [out]          LPSTR   lpBaseName,
  [in]           DWORD   nSize
);
*/
}

extern "C" {

SPLEAK_EXPORT void* sp_real_alloc(std::size_t size) {
    return resolved().malloc(size);
}

SPLEAK_EXPORT void sp_real_dealloc(void* p, std::size_t) {
    resolved().free(p);
}

SPLEAK_EXPORT void* malloc(std::size_t size) {
    return malloc_op(resolved().malloc(size), size);
}

SPLEAK_EXPORT void* calloc(std::size_t num, std::size_t size) {
    return calloc_op(resolved().calloc(num, size), num, size);
}

SPLEAK_EXPORT void* realloc(void* p, std::size_t newsize) {
    return realloc_op(resolved().realloc(p, newsize), p, newsize);
}

SPLEAK_EXPORT void free(void* p) {
    free_op(p);
    resolved().free(p);
}

}
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    using namespace securepath::spleak;
    context::instance().log("hops");
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            // Initialize once for each new process.
            if(!resolve_function_table()) {
                return FALSE;
            }
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
            if(reserved) {
                break; // do not do cleanup if process termination scenario
            }
            break;
    }
    return TRUE;
}

