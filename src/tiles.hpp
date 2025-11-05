#ifndef TILES_HPP
#define TILES_HPP

#include "core.hpp"
#include <string>
#include <vector>

using namespace Core;

class Tiles {
private:
	struct Tile {
		SDL_Rect rect {0, 0, 0, 0};
		std::string path_to_bmp;
	};
	int cols, size;
	std::vector<Tile> tiles;
	SDL_Color bg_col;

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

public:
	Tiles(int rows, int cols, int size, SDL_Color bg_col, int panel_w) :
		cols(cols), size(size), bg_col(bg_col)
	{
		for (int i = 0; i < rows * cols; i++) {
			Tile tile;
			tiles.push_back(tile);
		}
		distribute_tiles(panel_w);
	}

	auto render_data() {
		std::vector<RenderData> data;
		for (const auto& t : tiles) {
			RenderData tile;
			tile.dstrect = t.rect;
			if (!t.path_to_bmp.empty()) {
				tile.col_or_path_to_tex = t.path_to_bmp;
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

	void update(
		std::pair<int, int> mouse_pos,
		bool left_click, std::string path_to_bmp,
		int panel_w
	) {
		distribute_tiles(panel_w);
		for (auto& tile : tiles) {
			if (
				mouse_pos.first >= tile.rect.x &&
				mouse_pos.first <= tile.rect.x + tile.rect.w &&
				mouse_pos.second >= tile.rect.y &&
				mouse_pos.second <= tile.rect.y + tile.rect.h
		    ) {
				if (left_click) {
					tile.path_to_bmp = path_to_bmp;
				}
			}
		}
	}

};

#endif
