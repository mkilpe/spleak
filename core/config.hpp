#pragma once

#if defined(_MSC_VER) || defined(__MINGW32__)
#define SPLEAK_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define SPLEAK_EXPORT __attribute__((visibility("default")))
#else
#error "Undefined system, please add configuration..."
#endif