#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "core.hpp"
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

namespace Browser {

using namespace Core;

class Panel {

private:

	float panel_width_multiplier;
	SDL_Color panel_col;
	SDL_Rect panel;
	std::vector<std::string> paths_to_bmps;

public:
	void update_size(std::pair<int, int> win_size) {
		panel.w = static_cast<int>(static_cast<float>(win_size.first) * panel_width_multiplier);
		panel.h = win_size.second;
	}
	Panel(
		std::pair<int, int> win_size,
		float panel_width_multiplier,
		SDL_Color panel_col,
		std::filesystem::path path
	) :
		panel_width_multiplier(panel_width_multiplier), panel_col(panel_col)
	{
		update_size(win_size);
		panel.x = 0;
		panel.y = 0;

		// Get bmps

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
	}

	auto render_data() {
		RenderData data;
		data.dstrect = panel;
		data.col_or_path_to_tex = panel_col;
		return data;
	}

};

}

#endif
