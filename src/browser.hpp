/** @file src/browser.hpp */

#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "core.hpp"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

using namespace Core;

class Browser {

private:

	float panel_width_multiplier;
	SDL_Color panel_col;
	SDL_Rect panel;
	std::vector<std::string> paths_to_bmps;
	std::vector<SDL_Rect> thumbnails;
	int thumbnails_offset {0};

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
			t.x = 0;
			t.y = panel.w * i + thumbnails_offset;
			t.w = panel.w;
			t.h = panel.w;
			i++;
		}
	}

public:

	void update(std::pair<int, int> win_size, int scroll_speed, std::pair<int,int> mouse_pos) {
		if (mouse_pos.first < panel.w)
			thumbnails_offset += scroll_speed;
		if (thumbnails.front().y > 0)
			// thumbnails_offset--;
			thumbnails_offset -= 5;
		if (thumbnails.back().y < panel.h - panel.w)
			// thumbnails_offset++;
			thumbnails_offset += 5;
		if (std::abs(thumbnails_offset) <= 5)
			thumbnails_offset = 0;
		set_panel_size(win_size);
		set_thumbnails_size();
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
					paths_to_bmps.push_back(s);
					thumbnails.push_back({0, 0, 0, 0});
				}
			}

			DBGMSG(paths_to_bmps.size() << " bmp files found in directory.");

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

		int i = 0;
		for (const auto& t : thumbnails) {
			RenderData thumbnail;
			thumbnail.dstrect = t;
			thumbnail.col_or_path_to_tex = paths_to_bmps.at(static_cast<std::size_t>(i));
			data.push_back(thumbnail);
			i++;
		}

		return data;
	}

	auto get_paths_to_bmps() {
		return paths_to_bmps;
	}
};

#endif
