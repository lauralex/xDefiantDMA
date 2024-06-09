#include <iostream>
#include <thread>
#include <chrono>

#include "kmboxnet.h"
#include "aimbot.h"
#include "globals.h"
#include "utils.h"
#include "player.h"
#include "Memory/Memory.h"

// Chrono literals
using namespace std::chrono_literals;

// Kmbox variables
auto ip = "192.168.2.188";
auto port = "8808";
auto mac = "529B5C53";

// Debug variables
auto debug_position = false;

int main()
{
	if (!mem.Init("XDefiant.exe", true, false))
	{
		std::cout << "Failed to initialize memory" << '\n';
		return 1;
	}

	// Initialize keyboard
	if (!mem.GetKeyboard()->InitKeyboard())
	{
		std::cout << "Failed to initialize keyboard" << '\n';
		return 1;
	}

	// Initialize kmbox
	if (kmNet_init(ip, port, mac))
	{
		std::cout << "Failed to initialize kmbox" << '\n';
		return 1;
	}

	std::cout << "Initialized kmbox" << '\n';

	// Get the base address of the module and store it in the global variable
	globals::module_base = mem.GetBaseDaddy("XDefiant.exe");

	// Populate all the other global variables
	populate_globals();

	auto scatter_handle = mem.CreateScatterHandle();

	// Main loop
	while (true)
	{
		RUN_EVERY_START(5ms)
			populate_globals();
		RUN_EVERY_END;

		// RUN_EVERY_START(1ms)
		// 	if (is_memory_pointer_valid(globals::local_player))
		// 	{
		// 		// Get view matrix
		// 		cache::view_matrix = get_matrix();
		// 	}
		// RUN_EVERY_END;

		RUN_EVERY_START(200ms)
			if (debug_position && is_memory_pointer_valid(globals::local_player))
				cache::local_position = get_local_position();

			if (debug_position && is_memory_pointer_valid(globals::local_player))
				std::cout << "Position: " << cache::local_position.x << ", " << cache::local_position.y << ", " <<
					cache::local_position.z << '\n';
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Populate player list in the cache

			// Reserve big size for player list vector
			cache::player_list.reserve(100);

			// Clear the player list
			cache::player_list.clear();

			// Get the decrypted player list
			const auto decrypted_player_list = get_decrypted_player_list();

			// Get player list size
			const auto player_list_size = get_player_list_size();

			// Populate the player list
			for (int i = 0; i < player_list_size; ++i)
			{
				cache::player_list.push_back(std::make_shared<player>(scatter_handle, i, decrypted_player_list));
			}
			mem.ExecuteReadScatter(scatter_handle);
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Update players phase 1
			for (const auto& player : cache::player_list)
			{
				// If player is local player, save it to local_player
				if (player->is_equal(globals::local_player))
				{
					cache::local_player = player;
				}
				player->update_1(scatter_handle);
			}
			mem.ExecuteReadScatter(scatter_handle);
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Update players phase 2
			for (const auto& player : cache::player_list)
			{
				player->update_2(scatter_handle);
			}
			mem.ExecuteReadScatter(scatter_handle);
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Update players phase 3
			for (const auto& player : cache::player_list)
			{
				player->update_3(scatter_handle);
			}
			mem.ExecuteReadScatter(scatter_handle);
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Update players phase 4 and 5
			for (const auto& player : cache::player_list)
			{
				player->update_4();
				player->update_5(scatter_handle);
			}
			mem.ExecuteReadScatter(scatter_handle);

			// Remove player from list it is in the same team as local player
			std::erase_if(cache::player_list,
			              [](const auto& player)
			              {
				              return player->is_teammate(*cache::local_player) || player->
					              is_equal(*cache::local_player) || player->is_dead();
			              });
		RUN_EVERY_END;

		RUN_EVERY_START(0ms)
			// Aimbot routine
			aimbot();
		RUN_EVERY_END;


		// std::this_thread::sleep_for(1ms); // Should not be necessary, since work is done in the RUN_EVERY_START macro
	}
}
