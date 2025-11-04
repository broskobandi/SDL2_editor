#include <ctest.h>
#include <iostream>
#include <stdexcept>
#include "core.hpp"

int main(void) {
	try {
	[[maybe_unused]] Core::Sdl sdl;
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
	}
	ctest_print_results();
	return 0;
}
