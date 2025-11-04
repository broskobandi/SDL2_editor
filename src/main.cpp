/** @file src/main.cpp */

#include "core.hpp"
#include "browser.hpp"
#include <iostream>
#include <stdexcept>

using namespace Core;

int main(void) {

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

		while (sdl.get_is_running()) {
			sdl.poll_events();
			sdl.clear({30, 70, 70, 255});

			browser.update(sdl.win_size(), sdl.get_scroll_speed(), sdl.get_mouse_pos());
			sdl.draw(browser.render_data());
			sdl.present();
		}

	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
		std::cerr << SDL_GetError() << "\n";
	}

	return 0;
}
