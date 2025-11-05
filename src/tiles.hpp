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

/** @file src/tiles.hpp
 * @brief Private header file for the Tiles class. 
 * @details This file contains the definition of the Tiles class which is 
 * responsible for creating and saving the tile map. */

#ifndef TILES_HPP
#define TILES_HPP

#include "core.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace Core;
using json = nlohmann::json;

/** POD struct that stores data for a single tile. */
struct Tile {
	/** The tile rect. */
	SDL_Rect rect {0, 0, 0, 0};
	/** Path to the bmp to be rendered in the tile. */
	std::string path_to_bmp;
	/** Boolean representing whether or not the tile's texture has been set. */
	bool is_set {false};
	/** The angle by which the texture should be rotated. */
	float angle {0.0f};
	/** Flip state of the texture. */
	SDL_RendererFlip flip {SDL_FLIP_NONE};
};

/** Function defining the desired layout into the json file. */
inline void to_json(json& j, const Tile& t) {
	j = json{
		{"x", t.rect.x},
		{"y", t.rect.y},
		{"w", t.rect.w},
		{"h", t.rect.h},
		{"path_to_bmp", t.path_to_bmp},
		{"angle", t.angle}, {"flip", t.flip}
	};
}

/** Function defining the desired layout out of the json file. */
inline void from_json(const json& j, Tile& t) {
	j.at("x").get_to(t.rect.x);
	j.at("y").get_to(t.rect.y);
	j.at("w").get_to(t.rect.w);
	j.at("h").get_to(t.rect.h);
	j.at("path_to_bmp").get_to(t.path_to_bmp);
	j.at("angle").get_to(t.angle);
	j.at("flip").get_to(t.flip);
}

class Tiles {
private:

	// Private variables.

	int cols, size;
	std::vector<Tile> tiles;
	SDL_Color bg_col;

	// Private methods.

	/** Distributes the tiles based on the current panel width.
	 * @param panel_w The current panel width. */
	void distribute_tiles(int panel_w) {
		int cur_row = 0;
		int cur_col = 0;
		for (auto& t : tiles) {
			t.rect.x = cur_col * size + panel_w;
			t.rect.y = cur_row * size;
			t.rect.w = size;
			t.rect.h = size;
			cur_col++;
			if (cur_col % cols == 0) {
				cur_row++;
				cur_col = 0;
			}
		}
	}

	/** Saves the current map layout into a .json file.
	 * @throws std::runtime_error on failure. */
	void save() {
		json j = tiles;
		std::ofstream file("tiles.json"); // this should not be hardcoded!
		if (!file.is_open())
			throw std::runtime_error("Failed to save .json file");
		file << j.dump(4);
	}

public:

	/** Constructor for the Tiles class.
	 * @param rows The numbe rof rows in the map.
	 * @param cols The number of columns in the map.
	 * @param bg_col The background color.
	 * @param panel_w The current width of the panel. */
	Tiles(int rows, int cols, int size, SDL_Color bg_col, int panel_w) :
		cols(cols), size(size), bg_col(bg_col)
	{
		for (int i = 0; i < rows * cols; i++) {
			Tile tile;
			tiles.push_back(tile);
		}
		distribute_tiles(panel_w);
	}

	/** Returns the most up-to-date rendering context to be drawn. */
	auto render_data() {
		std::vector<RenderData> data;
		for (const auto& t : tiles) {
			RenderData tile;
			tile.dstrect = t.rect;
			if (!t.path_to_bmp.empty()) {
				tile.col_or_path_to_tex = t.path_to_bmp;
				tile.angle = t.angle;
				tile.flip = t.flip;
				data.push_back(tile);
			} else {
				tile.col_or_path_to_tex = SDL_Color{0, 0, 0, 255};
				RenderData inner_rect;
				inner_rect.dstrect = {t.rect.x + 1, t.rect.y + 1, t.rect.w - 2, t.rect.h - 2};
				inner_rect.col_or_path_to_tex = bg_col;
				data.push_back(tile);
				data.push_back(inner_rect);
			}
		}
		return data;
	}

	/** Updates all the tiles based on window size, panel width and user input.
	 * @param mouse_pos The current mouse position.
	 * @param left_click The current state of the left mouse button.
	 * @param path_to_bmp The path to the currently selected bmp.
	 * @param panel_w The current panel width.
	 * @param f_key The current state of the f key.
	 * @param r_key The current state of the r key.
	 * @param s_key The current state of the s key. */
	void update(
		std::pair<int, int> mouse_pos,
		bool left_click, std::string path_to_bmp,
		int panel_w,
		bool f_key, bool r_key, bool s_key
	) {

		distribute_tiles(panel_w);

		for (auto& tile : tiles) {
			if (
				mouse_pos.first >= tile.rect.x &&
				mouse_pos.first <= tile.rect.x + tile.rect.w &&
				mouse_pos.second >= tile.rect.y &&
				mouse_pos.second <= tile.rect.y + tile.rect.h
		    ) {
				if (!tile.is_set)
					tile.path_to_bmp = path_to_bmp;
				if (r_key)
					tile.angle += 90.0f;
				if (f_key) {
					if (static_cast<int>(tile.flip) < 2) {
						int f = static_cast<int>(tile.flip);
						f++;
						tile.flip = static_cast<SDL_RendererFlip>(f);
					} else {
						tile.flip = SDL_FLIP_NONE;
					}
				}
				if (left_click) {
					tile.is_set = true;
					tile.path_to_bmp = path_to_bmp;
				}
			} else {
				if (!tile.is_set)
					tile.path_to_bmp.clear();
			}
		}

		if (s_key) {
			save();
		}
	}

};

#endif
