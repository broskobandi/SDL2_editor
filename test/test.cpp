#include <ctest.h>
#include <iostream>
#include <stdexcept>
#include "core.hpp"
#include "browser.hpp"

using namespace Core;
using namespace Browser;

int main(void) {
	try {

		Core::Sdl sdl(
			SDL_INIT_EVERYTHING,
			"test",
			800, 600,
			SDL_WINDOW_SHOWN,
			SDL_RENDERER_PRESENTVSYNC
		);
		std::string path = "/home/broskobandi/Projects/SDL2_editor/test/assets/face.bmp";
		sdl.load_texture(path);
		RenderData data;
		data.dstrect = {0, 0, 50, 50};
		sdl.draw(data);
		data.col_or_path_to_tex = path;
		sdl.draw(data);

		Panel panel(sdl.win_size(), 0.1f, {100, 100, 100, 255}, "/home/broskobandi/Projects/SDL2_editor/test/assets");
		sdl.draw(panel.render_data());

	} catch (const std::runtime_error& e) {

		std::cerr << e.what() << "\n";
	}

	ctest_print_results();
	return 0;
}
