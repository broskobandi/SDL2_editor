#ifndef CORE_HPP
#define CORE_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>
#include <string_view>

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
		friend class Sdl;
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

	Base base;
	Window win;
	Renderer ren;

public:

	Sdl(Uint32 init_flags, std::string_view title, int w, int h, Uint32 win_flags) :
		base(init_flags),
		win(
			[&](){
				auto wi = SDL_CreateWindow(title.data(), 0, 0, w, h, win_flags);
				if (!wi) throw std::runtime_error("Failed to create window.");
				DBGMSG("Window created.");
				return wi;
			}(),
			[](SDL_Window* w){
				if (w) {
					SDL_DestroyWindow(w);
					DBGMSG("Window destroyed.");
				}
			}
		),
		ren(
			[&](){
				auto r = SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_PRESENTVSYNC);
				if (!r) throw std::runtime_error("Failed to create renderer.");
				DBGMSG("Renderer created.");
				return r;
			}(),
			[](SDL_Renderer* r) {
				if (r) {
					SDL_DestroyRenderer(r);
					DBGMSG("Renderer destroyed.");
				}
			}
		)
	{}

};

}

#endif
