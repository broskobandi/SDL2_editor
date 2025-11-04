#ifndef CORE_HPP
#define CORE_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

#ifndef NDEBUG
#include <iostream>
#define DBGMSG(msg)\
	std::cout << msg << "\n";
#else
#define DBGMSG(msg)
#endif

namespace Core {

using Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;
using Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>;
using Texture = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>;
using Surface = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>;

class Sdl {
private:
	class Base {
		Base(Uint32 flags) {
			if (SDL_Init(flags))
				std::runtime_error("Failed to init SDL.");
			DBGMSG("SDL initialized.");
		}
		~Base() {
			SDL_Quit();
			DBGMSG("SDL terminated.");
		}
	};
};

}

#endif
