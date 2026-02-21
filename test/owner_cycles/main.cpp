
#include <spleak/spleak.hpp>

#include <iostream>
#include <cstdlib>

int main() {
	void* a = std::malloc(32);
	void* b = std::malloc(32);
	void* c = std::malloc(32);
	void* d = std::malloc(32);

	std::cout << "a=" << a << " b=" << b << " c=" << c << " d=" << d << std::endl;

	// Cycle that is cleaned up before shutdown — should NOT appear in report
	std::cout << "\n--- cycle cleaned up before shutdown ---" << std::endl;
	spleak_add_pointer_owner(a, b, 32);
	spleak_add_pointer_owner(b, c, 32);
	spleak_add_pointer_owner(c, a, 32);
	std::cout << "removing C -> A (breaks cycle)" << std::endl;
	spleak_remove_pointer_owner(c, a);
	spleak_remove_pointer_owner(b, c);
	spleak_remove_pointer_owner(a, b);

	// Cycle via move that is left alive — should appear in report
	std::cout << "\n--- cycle alive at shutdown ---" << std::endl;
	spleak_add_pointer_owner(a, b, 32);
	spleak_add_pointer_owner(b, c, 32);
	spleak_move_pointer_owner(a, d, b);  // D -> B -> C
	spleak_add_pointer_owner(c, d, 32);  // C -> D closes cycle: D -> B -> C -> D
	std::cout << "cycle D -> B -> C -> D left alive" << std::endl;

	// Containment cycle left alive — should appear in report
	void* block = std::malloc(64);
	void* embedded_owner = static_cast<char*>(block) + 8;

	std::cout << "\n--- containment cycle alive at shutdown ---" << std::endl;
	std::cout << "block=" << block << " embedded_owner=" << embedded_owner << std::endl;
	spleak_add_pointer_owner(embedded_owner, a, 32);  // embedded_owner -> A
	spleak_add_pointer_owner(a, block, 64);           // A -> block (contains embedded_owner)
	std::cout << "cycle A -> block[embedded_owner] -> A left alive" << std::endl;

	// intentionally leak all cycle nodes so mem_ retains their allocation traces at shutdown
}
