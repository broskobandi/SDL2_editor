/*
MIT License

Copyright (c) 2025 broskobandi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/** @file src/core.hpp
 * @brief Private header file for the Core class. 
 * @details This file contains the definition of the Core class which is 
 * responsible for setting up the environment and managing the underlying 
 * SDL objects/tasks/events. */

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

// Custom types, structs and enums.

using Window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;
using Renderer = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>;
using Texture = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>;
using Surface = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>;

/** POD struct that contains the rendering context of a particular object. */
struct RenderData {
	/** The portion of the texture to be rendered (or the full texture if nullopt). */
	std::optional<SDL_Rect> srcrect {std::nullopt};
	/** The portion of the screen to be rendered on (or the full render target if nullopt). */
	std::optional<SDL_Rect> dstrect {std::nullopt};
	/** The color or texture to be rendered on the target.  */
	std::variant<SDL_Color, std::string> col_or_path_to_tex {SDL_Color{0, 0, 0, 255}};
	/** The angle with which the texture should be rotated by. */
	float angle {0.0f};
	/** The texture's flip state. */
	SDL_RendererFlip flip {SDL_FLIP_NONE};
};

/** Class to manage SDL objects and functionalities. */
class Sdl {

private:

	/** SDL session initializer class. */
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

	// Private variables.

	Base base;
	Window win;
	Renderer ren;
	std::map<std::string, Texture> textures_map;
	bool is_running {true};
	int scroll_state {0};
	std::pair<int, int> mouse_pos;
	bool left_click {false};
	bool f_key {false};
	bool r_key {false};
	bool s_key {false};

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
	
	/** Sets the color of the renderer.
	 * @param col The color or the renderer.
	 * @throws std::runtime_error on failure. */
	void set_draw_color(SDL_Color col) {
		if (SDL_SetRenderDrawColor(ren.get(), col.r, col.g, col.b, col.a))
			throw std::runtime_error("Failed to set draw color.");
	}

	/** Clears the renderer with the specified color.
	 * @param col The color to be used.
	 * @throws std::runtime_error on failure. */
	void clear(SDL_Color col) {
		set_draw_color(col);
		if (SDL_RenderClear(ren.get()))
			throw std::runtime_error("Failed to clear renderer.");
	}

	/** Presents the renderer. */
	void present() {
		SDL_RenderPresent(ren.get());
	}

	/** Queries the current window size.
	 * @return The window dimensions as std::pair<int, int> */
	std::pair<int, int> win_size() {
		int w, h;
		SDL_GetWindowSize(win.get(), &w, &h);
		return {w, h};
	}

	/** Loads and sotres a texture created from a bmp file if the given
	 * texture has not been created yet.
	 * @param path_to_bmp Path to the bmp file.
	 * @throws std::runtime_error on failure.  */
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

	/** Loads and sotres textures created from the bmp files if the given
	 * textures have not been created yet.
	 * @param path_to_bmps A vector of the paths to the bmp files.
	 * @throws std::runtime_error on failure.  */
	void load_texture(std::vector<std::string> paths_to_bmps) {
		for (const auto& path : paths_to_bmps) {
			load_texture(path);
		}
	}

	/** Polls SDL events and updates internal variables. */
	void poll_events() {
		bool is_scrolling = false;
		left_click = false;
		r_key = false;
		f_key = false;
		s_key = false;
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
						is_running = false;
					if (event.key.keysym.sym == SDLK_f)
						f_key = true;
					if (event.key.keysym.sym == SDLK_r)
						r_key = true;
					if (event.key.keysym.sym == SDLK_s)
						s_key = true;
					break;
				case SDL_MOUSEWHEEL:
					is_scrolling = true;
					scroll_state += event.wheel.y;
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
			if (scroll_state > 0)
				scroll_state--;
			if (scroll_state < 0)
				scroll_state++;
		}
	}

	/** Get a boolean representing the inner state of the engine */
	bool get_is_running() {
		return is_running;
	}

	/** Get the current mouse scroll state. 
	 * @return 1 if scrolling down, -1 if scrolling up, otherwise 0. */
	int get_scroll_state() {
		return scroll_state;
	}

	/** Get the current mouse position.
	 * @return The mouse position as std::pair<int, int>. */
	auto get_mouse_pos() {
		return mouse_pos;
	}

	/** Get the current state of the left mouse button.
	 * @return true if left mouse button is down, ortherwise false. */
	bool get_left_click() {
		return left_click;
	}

	/** Get the current state of the f key.
	 * @return true if the f key is down, ortherwise false. */
	bool get_f_key() {
		return f_key;
	}

	/** Get the current state of the r key.
	 * @return true if the r key is down, ortherwise false. */
	bool get_r_key() {
		return r_key;
	}

	/** Get the current state of the s key.
	 * @return true if the s key is down, ortherwise false. */
	bool get_s_key() {
		return s_key;
	}

	/** Draws the specified rendering context.
	 * @param data The rendering context to be drawn.
	 * @throws std::runtime_error on failure.  */
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

	/** Draws the specified rendering contexts.
	 * @param data A vector of rendering contexts to be drawn.
	 * @throws std::runtime_error on failure. */
	void draw(std::vector<RenderData> data) {
		for (const auto& d : data) {
			draw(d);
		}
	}
};

}

#endif
