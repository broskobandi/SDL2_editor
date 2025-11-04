/** @file src/core.hpp */

#ifndef CORE_HPP
#define CORE_HPP

#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

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

struct RenderData {
	std::optional<SDL_Rect> srcrect {std::nullopt};
	std::optional<SDL_Rect> dstrect {std::nullopt};
	std::variant<SDL_Color, std::string> col_or_path_to_tex {SDL_Color{0, 0, 0, 255}};
	float angle {0.0f};
	SDL_RendererFlip flip {SDL_FLIP_NONE};
};

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
	bool is_running {true};
	int scroll_speed {0};
	std::pair<int, int> mouse_pos;
	bool left_click {false};

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
			DBGMSG("Texture was loaded earlier for bmp: " << path_to_bmp);
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

	void load_texture(std::vector<std::string> paths_to_bmps) {
		for (const auto& path : paths_to_bmps) {
			load_texture(path);
		}
	}

	void poll_events() {
		bool is_scrolling = false;
		left_click = false;
		SDL_Event event;
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
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
						left_click = true;
					break;
				default: break;
			}
		}
		SDL_GetMouseState(&mouse_pos.first, &mouse_pos.second);
		if (!is_scrolling) {
			if (scroll_speed > 0)
				scroll_speed--;
			if (scroll_speed < 0)
				scroll_speed++;
		}
		// clear({30, 70, 70, 255});
	}

	bool get_is_running() {
		return is_running;
	}

	int get_scroll_speed() {
		return scroll_speed;
	}

	auto get_mouse_pos() {
		return mouse_pos;
	}

	bool get_left_click() {
		return left_click;
	}

	void draw(const RenderData& data) {
		const SDL_Rect* srcrect =
			data.srcrect.has_value() ? &data.srcrect.value() : nullptr;
		const SDL_Rect* dstrect =
			data.dstrect.has_value() ? &data.dstrect.value() : nullptr;
		if (std::holds_alternative<std::string>(data.col_or_path_to_tex)) {
			auto tex = textures_map.find(std::get<std::string>(data.col_or_path_to_tex));
			if (tex == textures_map.end())
				throw std::runtime_error("Failed to find texture.");
			if (
				SDL_RenderCopyEx(ren.get(), tex->second.get(), srcrect, dstrect, 
				data.angle, nullptr, data.flip)
			)
				throw std::runtime_error("Failed to render texture.");
			DBGMSG("Texture rendered.");
		} else if (std::holds_alternative<SDL_Color>(data.col_or_path_to_tex)) {
			SDL_Color col = std::get<SDL_Color>(data.col_or_path_to_tex);
			set_draw_color(col);
			if (SDL_RenderFillRect(ren.get(), dstrect))
				throw std::runtime_error("Failed to fill rect.");
			DBGMSG("Rect rendered.");
		}
	}

	void draw(std::vector<RenderData> data) {
		for (const auto& d : data) {
			draw(d);
		}
	}
};

}

#endif
