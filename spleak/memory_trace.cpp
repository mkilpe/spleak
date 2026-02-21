
#include "context.hpp"

namespace securepath::spleak {
extern "C" {

// add owner of address and size of the contained object if known
SPLEAK_EXPORT void spleak_add_pointer_owner(void const* owner_address, void const* containee_address, uint32_t size) {
	context::instance().add_pointer_owner(owner_address, containee_address, size);
}

// remove owner added with above function
SPLEAK_EXPORT void spleak_remove_pointer_owner(void const* owner_address, void const* containee_address) {
	context::instance().remove_pointer_owner(owner_address, containee_address);
}

// move owner, e.g. c++ move semantics
SPLEAK_EXPORT void spleak_move_pointer_owner(void const* old_owner, void const* new_owner, void const* containee_address) {
	context::instance().move_pointer_owner(old_owner, new_owner, containee_address);
}

}
}