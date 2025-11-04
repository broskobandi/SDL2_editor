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

	struct Thumbnail {
		SDL_Rect rect;
		std::string path_to_bmp;
		// bool is_selected {false};
		// bool is_highlighted = {false};
	};

	float panel_width_multiplier;
	SDL_Color panel_col;
	SDL_Rect panel;
	std::vector<Thumbnail> thumbnails;
	int thumbnails_offset {0};
	std::string selected_bmp;

	void set_panel_size(std::pair<int, int> win_size) {
		panel.w = static_cast<int>(static_cast<float>(win_size.first) * panel_width_multiplier);
		panel.h = win_size.second;
		panel.x = 0;
		panel.y = 0;
	}

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
					// if (!t.is_selected) {
						// t.is_selected = true;
						selected_bmp = t.path_to_bmp;
					} else {
						// t.is_selected = false;
						selected_bmp.clear();
					}
				}
			}
			// if (t.is_selected) {
			// 	t.rect.w -= t.rect.w / 10;
			// 	t.rect.x += t.rect.w / 20;
			// 	t.rect.h -= t.rect.w / 10;
			// 	t.rect.y += t.rect.w / 20;
			// }
		}
	}

public:

	void update(
		std::pair<int, int> win_size,
		int scroll_speed,
		std::pair<int,int> mouse_pos,
		bool left_click
	) {
		if (mouse_pos.first < panel.w)
			thumbnails_offset += scroll_speed;
		if (thumbnails.front().rect.y > 0)
			//let's not hardcode these..
			thumbnails_offset -= 5;
		if (thumbnails.back().rect.y < panel.h - panel.w)
			thumbnails_offset += 5;
		if (std::abs(thumbnails_offset) <= 5)
			thumbnails_offset = 0;
		set_panel_size(win_size);
		set_thumbnails_size();
		set_thumbnail_highlight(mouse_pos, left_click);
	}

	Browser(
		std::pair<int, int> win_size,
		float panel_width_multiplier,
		SDL_Color panel_col,
		std::filesystem::path path
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

	auto get_paths_to_bmps() {
		std::vector<std::string> paths;
		for (const auto& t: thumbnails) {
			paths.push_back(t.path_to_bmp);
		}
		return paths;
	}
};

#endif
