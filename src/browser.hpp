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

/** @file src/browser.hpp
 * @brief Private header file for the Browser class. 
 * @details This file contains the definition of the Browser class which is 
 * responsible for parsing and loading, presenting, and seleceting bmp files. */
/** @file src/browser.hpp */

#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "core.hpp"
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

using namespace Core;

class Browser {

private:

	/** POD struct that contains data for a single thumbnail. */
	struct Thumbnail {
		/** The rect to render the thumbnail over. */
		SDL_Rect rect;
		/** Path to the bmp to be rendered over the thumbnail. */
		std::string path_to_bmp;
	};

	// Private variables.

	float panel_width_multiplier;
	SDL_Color panel_col;
	SDL_Rect panel;
	std::vector<Thumbnail> thumbnails;
	int thumbnails_offset {0};
	std::string selected_bmp;

	// Private methods

	/** Sets the panel size based on the window size and the panel_width_multiplier.
	 * @param win_size The current size of the window. */
	void set_panel_size(std::pair<int, int> win_size) {
		panel.w = static_cast<int>(static_cast<float>(win_size.first) * panel_width_multiplier);
		panel.h = win_size.second;
		panel.x = 0;
		panel.y = 0;
	}

	/** Sets the size of the thumbnails based on the panel size.
	 * @throws std::runtime_error if there are no thumbnails available. */
	void set_thumbnails_size() {
		if (!thumbnails.size())
			throw std::runtime_error("Thumbnails vector is empty.");
		int i = 0;
		for (auto& t : thumbnails) {
			t.rect.x = 0;
			t.rect.y = panel.w * i + thumbnails_offset;
			t.rect.w = panel.w;
			t.rect.h = panel.w;
			i++;
		}
	}

	/** Handles thumbnail highlighting.
	 * @param mouse_pos The current position of the mouse.
	 * @param left_click The current state of the left mouse button. */
	void set_thumbnail_highlight(std::pair<int,int> mouse_pos, bool left_click) {
		for (auto& t : thumbnails) {
			if (
				mouse_pos.first < panel.w &&
				mouse_pos.second < t.rect.y + t.rect.w &&
				mouse_pos.second > t.rect.y
			) {
				t.rect.w += t.rect.w / 10;
				t.rect.x -= t.rect.w / 20;
				t.rect.h += t.rect.w / 10;
				t.rect.y -= t.rect.w / 20;
				if (left_click) {
					if (t.path_to_bmp != selected_bmp) {
						selected_bmp = t.path_to_bmp;
					} else {
						selected_bmp.clear();
					}
				}
			}
		}
	}

public:

	/** Updates all elements in the browser.
	 * @param win_size The current window size.
	 * @param scroll_state The current state of the mouse wheel.
	 * @param mouse_pos The current mouse position.
	 * @param left_click The current state of the left mouse button. */
	void update(
		std::pair<int, int> win_size,
		int scroll_state,
		std::pair<int,int> mouse_pos,
		bool left_click
	) {
		if (mouse_pos.first < panel.w)
			thumbnails_offset += scroll_state;
		if (thumbnails.front().rect.y > 0)
			thumbnails_offset -= 5;
		if (thumbnails.back().rect.y < panel.h - panel.w)
			thumbnails_offset += 5;
		if (std::abs(thumbnails_offset) <= 5)
			thumbnails_offset = 0;
		set_panel_size(win_size);
		set_thumbnails_size();
		set_thumbnail_highlight(mouse_pos, left_click);
	}

	/** Constructor for the Browser class.
	 * @param win_size The current size of the window.
	 * @param panel_width_multiplier The panel's size compared to the window size.
	 * @param panel_col The panel's background color.
	 * @param path The path of the working directory. */
	Browser(
		std::pair<int, int> win_size,
		float panel_width_multiplier,
		SDL_Color panel_col,
		std::filesystem::path path // this should be passed in as a command line argument.
	) :
		panel_width_multiplier(panel_width_multiplier), panel_col(panel_col)
	{
		set_panel_size(win_size);

		try {

			std::filesystem::directory_iterator dir_iter(path);

			for (const auto& entry : dir_iter) {
				auto s = entry.path().string();
				if (s.substr(s.length() - 4, 4) == ".bmp") {
					// paths_to_bmps.push_back(s);
					Thumbnail t;
					t.path_to_bmp = s;
					t.rect = {0, 0, 0, 0};
					thumbnails.push_back(t);
					// thumbnails.push_back({0, 0, 0, 0});
				}
			}

			DBGMSG(thumbnails.size() << " bmp files found in directory.");

		} catch (const std::filesystem::filesystem_error&) {
			throw std::runtime_error("Invalid path.");
		}

		set_thumbnails_size();
	}

	/** Creates and returns the most up-to-date rendering context. */
	auto render_data() {
		std::vector<RenderData> data;
		
		// Panel
		
		RenderData panel_data;
		panel_data.dstrect = panel;
		panel_data.col_or_path_to_tex = panel_col;
		data.push_back(panel_data);

		// Thumbnails

		for (const auto& t : thumbnails) {
			RenderData thumbnail;
			SDL_Rect rect = t.rect;
			if (t.path_to_bmp == selected_bmp) {
				rect.w -= rect.w / 10;
				rect.x += rect.w / 20;
				rect.h -= rect.w / 10;
				rect.y += rect.w / 20;
			}
			thumbnail.dstrect = rect;
			thumbnail.col_or_path_to_tex = t.path_to_bmp;
			data.push_back(thumbnail);
		}

		return data;
	}

	/** Returns the path of the currently selected bmp. */
	std::string get_selected_bmp() {
		return selected_bmp;
	}

	/** Returns the current width of the side panel. */
	int get_panel_w() {
		return panel.w;
	}

	/** Returns a vector of the paths to all the bmp files found in the working directory. */
	auto get_paths_to_bmps() {
		std::vector<std::string> paths;
		for (const auto& t: thumbnails) {
			paths.push_back(t.path_to_bmp);
		}
		return paths;
	}
};

#endif
