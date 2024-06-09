#pragma once
#include "globals.h"
#include "kmboxnet.h"
#include "utils.h"


inline void aimbot()
{
	// Chrono literals
	using namespace std::chrono_literals;

	// Get distance of each player from local player and choose the closest one
	static bool aimbot_button_pressed = false;

	RUN_EVERY_START(100ms)
		if (mem.GetKeyboard()->IsKeyDown(VK_RBUTTON))
		{
			aimbot_button_pressed = true;
		}
		else
		{
			aimbot_button_pressed = false;
		}
	RUN_EVERY_END;

	if (!aimbot_button_pressed) return;
	update_matrix();

	float closest_distance = 999999999.0f;
	std::shared_ptr<player> aimbot_target = nullptr;
	for (const auto& player : cache::player_list)
	{
		// Skip if the player is the local player
		if (player->is_equal(globals::local_player)) continue;

		if (vector2_t screen_position; world_to_screen(player->get_player_position(), screen_position))
		{
			const auto distance = player->get_player_position().distance(cache::local_player->get_player_position());
			const auto screen_distance = screen_position.distance({
				static_cast<float>(globals::screen_width) / 2.0f, static_cast<float>(globals::screen_height) / 2.0f
			});
			if (const auto weighted_distance =  screen_distance * 0.80f + distance * 0.20f; weighted_distance <
				closest_distance)
			{
				closest_distance = weighted_distance;
				aimbot_target = player;
			}
		}
	}

	// If we have a target, aim at it
	if (aimbot_target)
	{
		// Check if is dead
		if (aimbot_target->is_dead())
		{
			aimbot_target = nullptr;
			return;
		}

		// Get the bone position of the target player
		const auto bone_position = aimbot_target->get_player_position();

		// Get world to screen position
		if (vector2_t screen_position; world_to_screen(bone_position, screen_position))
		{
			// Get the center of the screen

			// kmbox move mouse to screen position if difference is greater than 2 pixels
			if (const auto [x_center, y_center] = vector2_t{
				static_cast<float>(globals::screen_width) / 2.0f, static_cast<float>(globals::screen_height) / 2.0f
			}; std::abs(screen_position.x - x_center) > 2.0f || std::abs(screen_position.y - y_center) > 2.0f)
			{
				// Move mouse to the target using delta
				kmNet_mouse_move((screen_position.x - x_center) / 5.0f, (screen_position.y - y_center) / 5.0f);
			}
		}
	}
}
