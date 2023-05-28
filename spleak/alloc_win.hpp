
#include "alloc.hpp"
#include "config.hpp"
#include "context.hpp"

//#ifdef _MSC_VER

#include <cstring>

#include <windows.h>
#include <psapi.h>

namespace securepath::spleak {
namespace {

    //todo: first check for correct names, ie.  MSVCR71.DLL, MSVCR80.DLL, MSVCR90.DLL, MSVCR100.DLL, so on.
    constexpr std::string_view dll_names[] = { "msvcrt.dll" };

    struct dll_init_data {
        std::atomic_flag flag{};
        static_alloc<4096> alloc;
    };

    dll_init_data& dll_init() {
        static dll_init_data init;
        return init;
    }

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

    void convert_to_lowercase(char* str, std::size_t s) {
        for(auto* p = str; p != str+s ; ++p) {
            if(*p >= 'A' && *p <= 'Z') {
                *p = *p - 'A' + 'a';
            }
        }
    }

    bool dll_name_matches(std::string_view name) {
        for(auto&& v : dll_names) {
            if(name == v) {
                return true;
            }
        }
        return false;
    }

    HMODULE find_module() {
        HMODULE ret{};
        HMODULE mods[4*1024];
        HANDLE process = GetCurrentProcess();
        DWORD mod_handle_bytes{};
        char name[32*1024];

        if(!EnumProcessModules(process, mods, sizeof(mods), &mod_handle_bytes)) {
            context::instance().log("EnumProcessModules failed (err = {})", GetLastError());
            return nullptr;
        }

        for(std::size_t i = 0; i != (mod_handle_bytes / sizeof(HMODULE)) && !ret; ++i) {
            if(auto len = GetModuleFileNameA(mods[i], name, sizeof(name))) {
                std::string_view s(name, name+len);
                context::instance().log("-- {}", name);
                auto p = s.find_last_of("\\/");
                auto* data = name;
                if(p != std::string_view::npos) {
                    s = s.substr(p+1);
                    data += p+1;
                }
                convert_to_lowercase(data, s.size());
                if(dll_name_matches(s)) {
                    ret = mods[i];
                }
            }
        }
        return ret;
    }

    bool resolve_function_table() {
        HMODULE m = find_module();
        // perhaps later on try other strategies if fails to find the module based on dll name?
        return m && resolve_functions(m, impl());
    }
}

extern "C" {

SPLEAK_EXPORT void* sp_real_alloc(std::size_t size) {
    return resolved().malloc(size);
}

SPLEAK_EXPORT void sp_real_dealloc(void* p, std::size_t) {
    resolved().free(p);
}

SPLEAK_EXPORT void* malloc(std::size_t size) {
    auto& d = dll_init();
    if(d.flag.test()) {
        return malloc_op(resolved().malloc(size), size);
    } else {
        return d.alloc.alloc(size, alignof(std::max_align_t));
    }
}

SPLEAK_EXPORT void* calloc(std::size_t num, std::size_t size) {
    auto& d = dll_init();
    if(d.flag.test()) {
        return calloc_op(resolved().calloc(num, size), num, size);
    } else {
        auto p = d.alloc.alloc(size*num, alignof(std::max_align_t));
        if(p) {
            std::memset(p, 0, size*num);
        }
        return p;
    }
}

SPLEAK_EXPORT void* realloc(void* p, std::size_t newsize) {
    return realloc_op(resolved().realloc(p, newsize), p, newsize);
}

SPLEAK_EXPORT void free(void* p) {
    if(!dll_init().alloc.in_range(p)) {
        free_op(p);
        resolved().free(p);
    }
}

}
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    using namespace securepath::spleak;
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            if(!resolve_function_table()) {
                return FALSE;
            }
            dll_init().flag.test_and_set();
            break;

        case DLL_PROCESS_DETACH:
            if(scoped_internal_op _{}) {
                context::instance().report_on_shutdown();
            }
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}
