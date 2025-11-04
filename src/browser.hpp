#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "core.hpp"
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

public:
	void update(std::pair<int, int> win_size) {
		panel.w = static_cast<int>(static_cast<float>(win_size.first) * panel_width_multiplier);
		panel.h = win_size.second;
		panel.x = 0;
		panel.y = 0;

		int i = 0;
		for (auto& t : thumbnails) {
			t.x = 0;
			t.y = i * panel.w;
			t.w = panel.w;
			t.h = panel.h;
		}
	}
	Browser(
		std::pair<int, int> win_size,
		float panel_width_multiplier,
		SDL_Color panel_col,
		std::filesystem::path path
	) :
		panel_width_multiplier(panel_width_multiplier), panel_col(panel_col)
	{
		try {

			std::filesystem::directory_iterator dir_iter(path);

			for (const auto& entry : dir_iter) {
				auto s = entry.path().string();
				if (s.substr(s.length() - 4, 4) == ".bmp")
					paths_to_bmps.push_back(s);
			}

			if (!paths_to_bmps.size()) {
				throw std::runtime_error("No bmp files found in directory.");
			} else {
				DBGMSG(paths_to_bmps.size() << " bmp files found in directory.");
			}

		} catch (const std::filesystem::filesystem_error&) {
			throw std::runtime_error("Invalid path.");
		}

		update(win_size);
	}

	auto render_data() {
		// Panel
		
		RenderData data;
		data.dstrect = panel;
		data.col_or_path_to_tex = panel_col;
		return data;

		// Ttextures
	}

	auto get_paths_to_bmps() {
		return paths_to_bmps;
	}
};

#endif
