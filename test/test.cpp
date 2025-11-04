#include <ctest.h>
#include <iostream>
#include <stdexcept>
#include "core.hpp"

int main(void) {
	try {
	Core::Sdl sdl(
		SDL_INIT_EVERYTHING,
		"test",
		800, 600,
		SDL_WINDOW_SHOWN,
		SDL_RENDERER_PRESENTVSYNC
	);
	sdl.load_texture("/home/broskobandi/Projects/SDL2_editor/test/assets/face.bmp");
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
	}
	ctest_print_results();
	return 0;
}
