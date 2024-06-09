#pragma once
#include <cstdint>
#include <mutex>

#include "player.h"

namespace globals
{
	inline int screen_width = 1920;
	inline int screen_height = 1080;
	inline uintptr_t module_base;
	inline uintptr_t client;
	inline uintptr_t world;
	inline uintptr_t local_player;
	inline uintptr_t view_data;
}

namespace cache
{
	inline std::vector<std::shared_ptr<player>> player_list;
	inline matrix4_t view_matrix;
	inline vector3_t local_position;
	inline std::shared_ptr<player> local_player;
}
