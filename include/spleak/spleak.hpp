
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void spleak_add_pointer_owner(const void* owner_address, const void* containee_address);
void spleak_remove_pointer_owner(const void* owner_address, const void* containee_address);

#ifdef __cplusplus
}
#endif