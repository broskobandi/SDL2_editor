#ifndef CORE_HPP
#define CORE_HPP

#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

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
	std::map<std::string, Texture> textures_map;

public:

	Sdl(
		Uint32 init_flags,
		std::string_view title,
		int w, int h,
		Uint32 win_flags, Uint32 ren_flags
	) :
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
				auto r = SDL_CreateRenderer(win.get(), -1, ren_flags);
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
	
	void set_draw_color(SDL_Color col) {
		if (SDL_SetRenderDrawColor(ren.get(), col.r, col.g, col.b, col.a))
			throw std::runtime_error("Failed to set draw color.");
	}

	void clear(SDL_Color col) {
		set_draw_color(col);
		if (SDL_RenderClear(ren.get()))
			throw std::runtime_error("Failed to clear renderer.");
	}

	void present() {
		SDL_RenderPresent(ren.get());
	}

	std::pair<int, int> win_size() {
		int w, h;
		SDL_GetWindowSize(win.get(), &w, &h);
		return {w, h};
	}

	void load_texture(std::string path_to_bmp) {
		if (textures_map.find(path_to_bmp) != textures_map.end()) {
			DBGMSG("Texture was loaded earlier.");
			return;
		}
		auto sur = Surface(
			[&](){
				auto s = SDL_LoadBMP(path_to_bmp.data());
				if (!s) throw std::runtime_error("Failed to load bmp.");
				DBGMSG("Loaded bmp: " << path_to_bmp);
				return s;
			}(),
			[](SDL_Surface* s) {
				if (s) {
					SDL_FreeSurface(s);
					DBGMSG("Surface freed.");
				}
			}
		);
		auto tex = Texture(
			[&](){
				auto t = SDL_CreateTextureFromSurface(ren.get(), sur.get());
				if (!t) throw std::runtime_error("Failed to create texture.");
				DBGMSG("Texture created.");
				return t;
			}(),
			[](SDL_Texture* t) {
				if (t) {
					SDL_DestroyTexture(t);
					DBGMSG("Texture destroyed.");
				}
			}
		);
		if (!textures_map.emplace(path_to_bmp, std::move(tex)).second)
			throw std::runtime_error("Failed to emplace new texture into map.");
		DBGMSG("New texture emplaced into map.");
	}
};

}

#endif
