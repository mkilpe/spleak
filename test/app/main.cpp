
#include <iostream>

int main() {
	void* test = malloc(4);
	std::cout << "allocated address " << test << std::endl;
	free(test);
}