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

		SDL_Event event;
		bool is_running = true;
		int scroll_speed = 0;
		bool is_scrolling = false;
		while (is_running) {
			is_scrolling = false;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_q)
							is_running = false;
						break;
					case SDL_MOUSEWHEEL:
						is_scrolling = true;
						scroll_speed += event.wheel.y;
						break;
					default: break;
				}
			}
			sdl.clear({30, 70, 70, 255});
			if (!is_scrolling) {
				if (scroll_speed > 0)
					scroll_speed--;
				if (scroll_speed < 0)
					scroll_speed++;
			}
			std::cout << scroll_speed << "\n";
			// int x, y;
			// SDL_GetMouseState(&x, &y);
			// browser.update(sdl.win_size(), scroll_speed);
			browser.update(sdl.win_size(), scroll_speed);
			sdl.draw(browser.render_data());
			sdl.present();
		}

	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
		std::cerr << SDL_GetError() << "\n";
	}

	return 0;
}
