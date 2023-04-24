
#include <iostream>
#include <cstdlib>

int main() {
	void* test = std::malloc(4);
	std::cout << "allocated address " << test << std::endl;
	std::free(test);

	[[maybe_unused]] void* unfreed = std::malloc(12);
}