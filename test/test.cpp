#include <ctest.h>
#include <iostream>
#include <stdexcept>
#include "core.hpp"
#include "browser.hpp"
#include "tiles.hpp"

using namespace Core;

int main(void) {
	try {

		Core::Sdl sdl(
			SDL_INIT_EVERYTHING,
			"test",
			800, 600,
			SDL_WINDOW_SHOWN,
			SDL_RENDERER_PRESENTVSYNC
		);

		Browser browser(
			sdl.win_size(),
			0.1f, {100, 100, 100, 255},
			"/home/broskobandi/Projects/SDL2_editor/test/assets");

		sdl.load_texture(browser.get_paths_to_bmps());

		browser.update(sdl.win_size(), 0, sdl.get_mouse_pos(), sdl.get_left_click());

		Tiles tiles(4, 4, 64, {30, 70, 70, 255}, browser.get_panel_w());

		tiles.update(sdl.get_mouse_pos(), sdl.get_left_click(), "", browser.get_panel_w(), sdl.get_f_key(), sdl.get_r_key());

		sdl.draw(tiles.render_data());

		sdl.draw(browser.render_data());

		CTEST(1);

	} catch (const std::runtime_error& e) {

		CTEST(0);
		std::cerr << e.what() << "\n";
	}

	ctest_print_results();
	return 0;
}
