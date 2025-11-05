#define NDEBUG
/** @file src/main.cpp */

#include "core.hpp"
#include "browser.hpp"
#include "tiles.hpp"
#include <iostream>
#include <stdexcept>

using namespace Core;

int main(void) {

	SDL_Color bg_col{30, 70, 70, 255};

	try {

		Sdl sdl(
			SDL_INIT_EVERYTHING,
			"Editor",
			800,
			600,
			SDL_WINDOW_SHOWN,
			SDL_RENDERER_PRESENTVSYNC
		);

		Browser browser(
			sdl.win_size(),
			0.1f,
			{100, 100, 100, 255},
			"/home/broskobandi/Projects/SDL2_editor/test/assets"
		);

		sdl.load_texture(browser.get_paths_to_bmps());

		// Tiles tiles(4, 4, 64, bg_col, browser.get_panel_w());
		Tiles tiles(4, 4, 64, {100, 100, 100, 255}, browser.get_panel_w());

		while (sdl.get_is_running()) {
			sdl.poll_events();
			sdl.clear(bg_col);

			browser.update(sdl.win_size(), sdl.get_scroll_speed(), sdl.get_mouse_pos(), sdl.get_left_click());
			// tiles.update(browser.get_panel_w());
			tiles.update(sdl.get_mouse_pos(), sdl.get_left_click(), browser.get_selected_bmp(), browser.get_panel_w(), sdl.get_f_key(), sdl.get_r_key());
			sdl.draw(browser.render_data());
			sdl.draw(tiles.render_data());
			sdl.present();
		}

	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
		std::cerr << SDL_GetError() << "\n";
	}

	return 0;
}
