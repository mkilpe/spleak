
#include "context.hpp"

namespace securepath::spleak {
extern "C" {

// add owner of address and size of the contained object if known
void spleak_add_pointer_owner(const void* owner_address, const void* containee_address, std::uint32_t size) {
	context::instance().add_pointer_owner(owner_address, containee_address, size);
}

// remove owner added with above function
void spleak_remove_pointer_owner(const void* owner_address, const void* containee_address) {
	context::instance().remove_pointer_owner(owner_address, containee_address);
}

// move owner, e.g. c++ move semantics
void spleak_move_pointer_owner(const void* old_owner, const void* new_owner, const void* containee_address) {
	context::instance().move_pointer_owner(old_owner, new_owner, containee_address);
}

}
}