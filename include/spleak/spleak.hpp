#pragma once

#include <stdint.h>
#include <core/config.hpp>

#ifdef __cplusplus
extern "C" {
#endif

SPLEAK_EXPORT void spleak_add_pointer_owner(void const* owner_address, void const* containee_address, uint32_t size);
SPLEAK_EXPORT void spleak_remove_pointer_owner(void const* owner_address, void const* containee_address);
SPLEAK_EXPORT void spleak_move_pointer_owner(void const* old_owner, void const* new_owner, void const* containee_address);

#ifdef __cplusplus
}
#endif