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

		SDL_Event event;
		bool is_running = true;
		while (is_running) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_q)
							is_running = false;
					default: break;
				}
			}
			sdl.clear({30, 70, 70, 255});
			browser.update_size(sdl.win_size());
			sdl.draw(browser.render_data());
			sdl.present();
		}

	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << "\n";
		std::cerr << SDL_GetError() << "\n";
	}

	return 0;
}
