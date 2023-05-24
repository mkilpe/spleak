
#include "context.hpp"

namespace securepath::spleak {
extern "C" {

void spleak_add_pointer_owner(const void* owner_address, const void* containee_address) {
	context::instance().add_pointer_owner(owner_address, containee_address);
}

void spleak_remove_pointer_owner(const void* owner_address, const void* containee_address) {
	context::instance().remove_pointer_owner(owner_address, containee_address);
}

void spleak_move_pointer_owner(const void* old_owner, const void* new_owner, const void* containee_address) {
	context::instance().move_pointer_owner(old_owner, new_owner, containee_address);
}

}
}