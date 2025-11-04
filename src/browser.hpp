#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "core.hpp"
#include <utility>

namespace Browser {

using namespace Core;

class Panel {

private:

	float panel_width_multiplier;
	SDL_Color panel_col;
	SDL_Rect panel;

public:
	void update_size(std::pair<int, int> win_size) {
		panel.w = static_cast<int>(static_cast<float>(win_size.first) * panel_width_multiplier);
		panel.h = win_size.second;
	}
	Panel(
		std::pair<int, int> win_size,
		float panel_width_multiplier,
		SDL_Color panel_col
	) :
		panel_width_multiplier(panel_width_multiplier), panel_col(panel_col)
	{
		update_size(win_size);
		panel.x = 0;
		panel.y = 0;
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
