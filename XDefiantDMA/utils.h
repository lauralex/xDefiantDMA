#pragma once
#include <cstdint>

#include "structs.h"
#include "globals.h"
#include "Memory/Memory.h"


// Macros to run code every x interval
#define RUN_EVERY_START(interval) \
    do { \
        static auto last_time = std::chrono::high_resolution_clock::now() - 99999s; \
        auto current_time = std::chrono::high_resolution_clock::now(); \
        auto elapsed = current_time - last_time; \
        if (elapsed >= (interval)) {

#define RUN_EVERY_END \
            last_time = current_time; \
        } \
    } while(0)

inline bool is_memory_pointer_valid(const uintptr_t address)
{
	return address != 0 && address != 0xCCCCCCCCCCCCCCCC && address != 0xCDCDCDCDCDCDCDCD && address < 0x7FFFFFFFFFFF &&
		address > 0x1000;
}

inline uintptr_t get_client()
{
	const uintptr_t client_encrypted = mem.Read<uintptr_t>(globals::module_base + 0x6322598, true);
	if (client_encrypted == 0x387D0A44CEFB6943)
	{
		return 0;
	}
	const uintptr_t client_encrypted2 = mem.Read<uintptr_t>((client_encrypted ^ 0x387D0A44CEFB6943) + 0x170, true);
	if (client_encrypted2 == 0xDAA5B91DD012614B)
	{
		return 0;
	}
	return client_encrypted2 + 0x255A46E22FED9EB5;
}

inline uintptr_t get_world()
{
	if (!is_memory_pointer_valid(globals::client)) return 0;
	return mem.Read<uintptr_t>(globals::client + 0x30, true) + 0x1715D45D50892E04;
}

inline uintptr_t get_local_player()
{
	const auto b = mem.Read<uintptr_t>(globals::world + 0x2f8, true) + 0xC8FFB474B02A158;
	const auto v1 = mem.Read<uintptr_t>(b + 0x10, true);
	if (!is_memory_pointer_valid(v1)) return 0;
	return mem.Read<uintptr_t>(v1 + 0x30, true);
}

inline uintptr_t decrypt_player_list(const uintptr_t encrypted)
{
	return encrypted + 0x24B166456C46541F;
}

inline uintptr_t get_decrypted_player_list()
{
	return decrypt_player_list(mem.Read<uintptr_t>(globals::world + 0x490, true));
}

inline uintptr_t get_player_by_id(const size_t index)
{
	const uintptr_t decrypted_player_list = get_decrypted_player_list();
	if (!is_memory_pointer_valid(decrypted_player_list)) return 0;
	return mem.Read<uintptr_t>(decrypted_player_list + index * 0x8);
}

inline uintptr_t get_player_by_id(const size_t index, const uintptr_t decrypted_player_list)
{
	if (!is_memory_pointer_valid(decrypted_player_list)) return 0;
	return mem.Read<uintptr_t>(decrypted_player_list + index * 0x8);
}

inline void get_player_by_id_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const size_t index,
                                     const uintptr_t decrypted_player_list,
                                     uintptr_t& player_ptr)
{
	if (!is_memory_pointer_valid(decrypted_player_list)) return;
	mem.AddScatterReadRequest(scatter_handle, decrypted_player_list + index * 0x8, &player_ptr, sizeof(uintptr_t));
}

inline int get_player_list_size()
{
	return mem.Read<int>(globals::world + 0x498, true);
}

inline uintptr_t get_view_data()
{
	const auto a1 = mem.Read<uintptr_t>(globals::world + 0x318, true) - 0x591ACDE1A27BAB86;
	if (!is_memory_pointer_valid(a1)) return 0;
	const auto v2 = mem.Read<uintptr_t>(a1 + 0x28, true) + 0x3AD084C9B8BDE624;
	if (!is_memory_pointer_valid(v2)) return 0;
	return v2;
}

inline matrix4_t get_matrix()
{
	return mem.Read<matrix4_t>(globals::view_data + 0x10);
}

inline void update_matrix()
{
	cache::view_matrix = get_matrix();
}

inline uintptr_t get_health_component(const uintptr_t player)
{
	return mem.Read<uintptr_t>(player + 0x8B0) - 0x419D4AF03F0C0BA7;
}

inline void get_temp_health_component_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t player,
                                              uintptr_t& health_component)
{
	mem.AddScatterReadRequest(scatter_handle, player + 0x8B0, &health_component, sizeof(uintptr_t));
}

inline uintptr_t get_health_component_from_temp(const uintptr_t temp_health_component)
{
	return temp_health_component - 0x419D4AF03F0C0BA7;
}

inline int get_health(const uintptr_t health_component)
{
	return mem.Read<int>(health_component + 0x5C);
}

inline void get_health_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t health_component, int& hp)
{
	mem.AddScatterReadRequest(scatter_handle, health_component + 0x5C, &hp, sizeof(int));
}

inline int get_team_id(const uintptr_t player)
{
	const auto v12 = mem.Read<uintptr_t>(player + 0x978) + 0x6B43BCEBF21EACAE - 0x10;
	if (!is_memory_pointer_valid(v12)) return 0;
	return mem.Read<int>(v12 + 0x10);
}

inline void get_temp_team_id_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t player,
                                     uintptr_t& temp_team_id)
{
	mem.AddScatterReadRequest(scatter_handle, player + 0x978, &temp_team_id, sizeof(uintptr_t));
}

inline void get_team_id_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t temp_team_id_address,
                                int& team_id)
{
	const auto v12 = temp_team_id_address + 0x6B43BCEBF21EACAE - 0x10;
	if (!is_memory_pointer_valid(v12))
	{
		team_id = 0;
		return;
	}

	mem.AddScatterReadRequest(scatter_handle, v12 + 0x10, &team_id, sizeof(int));
}

inline uintptr_t get_animation(const uintptr_t player)
{
	return _rotr64(mem.Read<uintptr_t>(player + 0x230), 25) ^ 0x7C904ACEACB2CA8F;
}

inline uintptr_t get_animation_from_temp(const uintptr_t temp_animation)
{
	return _rotr64(temp_animation, 25) ^ 0x7C904ACEACB2CA8F;
}

inline void get_temp_animation_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t player,
                                       uintptr_t& temp_animation_ptr)
{
	mem.AddScatterReadRequest(scatter_handle, player + 0x230, &temp_animation_ptr, sizeof(uintptr_t));
}

inline uintptr_t get_bone_address(const size_t bone_index, const uintptr_t animation)
{
	const uintptr_t bone_array = _rotr64(mem.Read<uintptr_t>(animation + 0x68) ^ 0xA123AD2DDEA34727, 0x15) -
		0xA5E10E02FB8D437;
	if (!is_memory_pointer_valid(bone_array))
		return mem.Read<uintptr_t>(animation + 0x40) + 0x40;
	if (bone_index < 0 || bone_index >= (mem.Read<uintptr_t>(bone_array + 0x58) & 0x7FFFFFFF))
	{
		return bone_array + 0x10;
	}
	const uintptr_t bone_encrypted = _rotr64(mem.Read<uintptr_t>(bone_array + 0x50) + 0x1E3FDAF6A35FD9E5, 0xD) +
		0x105B630EAC8B5E93;
	return (bone_index << 6) + bone_encrypted;
}

inline uintptr_t get_bone_address_from_array(uintptr_t bone_array, uintptr_t bone_address_unshifted,
                                             const size_t bone_index, const size_t array_size)
{
	if (!is_memory_pointer_valid(bone_array))
	{
		return 0;
	}
	if (bone_index < 0 || bone_index >= (array_size & 0x7FFFFFFF))
	{
		bone_array = mem.Read<uintptr_t>(bone_array + 0x10);
		bone_address_unshifted = mem.Read<uintptr_t>(bone_array + 0x50);
	}

	const auto bone_encrypted = _rotr64(bone_address_unshifted + 0x1E3FDAF6A35FD9E5, 0xD) +
		0x105B630EAC8B5E93;
	return (bone_index << 6) + bone_encrypted;
}

inline uintptr_t get_bone_array_address_from_temp(const uintptr_t temp_bone_array)
{
	return _rotr64(temp_bone_array ^ 0xA123AD2DDEA34727, 0x15) - 0xA5E10E02FB8D437;
}

inline void get_temp_bone_array_address(const VMMDLL_SCATTER_HANDLE scatter_handle, const uintptr_t animation,
                                        uintptr_t& temp_bone_array)
{
	mem.AddScatterReadRequest(scatter_handle, animation + 0x68, &temp_bone_array, sizeof(uintptr_t));
}

inline vector3_t get_bone_position(const uintptr_t bone_address)
{
	if (is_memory_pointer_valid(bone_address))
	{
		const auto matrix = mem.Read<matrix4_t>(bone_address);
		return vector3_t{matrix._41, matrix._42, matrix._43};
	}
	return vector3_t{0, 0, 0};
}

inline void get_bone_position_scatter(const VMMDLL_SCATTER_HANDLE scatter_handle, matrix4_t& matrix,
                                      const uintptr_t bone_address)
{
	if (is_memory_pointer_valid(bone_address))
	{
		mem.AddScatterReadRequest(scatter_handle, bone_address, &matrix, sizeof(matrix4_t));
	}
}

inline void populate_globals()
{
	globals::client = get_client();
	globals::world = get_world();
	globals::local_player = get_local_player();
	globals::view_data = get_view_data();
	//update_matrix();
}

inline vector3_t get_local_position()
{
	const auto animation = get_animation(globals::local_player);
	const auto bone_address = get_bone_address(0, animation);
	return get_bone_position(bone_address);
}

inline bool world_to_screen(const vector3_t& world_pos, vector2_t& screen_pos)
{
	const auto view_matrix = cache::view_matrix;

	const auto w = view_matrix._14 * world_pos.x + view_matrix._24 * world_pos.y + view_matrix._34 * world_pos.z +
		view_matrix._44;

	if (w < 0.00001f)
	{
		return false;
	}

	screen_pos.x = view_matrix._11 * world_pos.x + view_matrix._21 * world_pos.y + view_matrix._31 * world_pos.z +
		view_matrix._41;
	screen_pos.y = view_matrix._12 * world_pos.x + view_matrix._22 * world_pos.y + view_matrix._32 * world_pos.z +
		view_matrix._42;

	screen_pos.x /= w;
	screen_pos.x = (screen_pos.x + 1.0f) * 0.5f * globals::screen_width;
	screen_pos.y /= w;
	screen_pos.y = (1.0f - screen_pos.y) * 0.5f * globals::screen_height;

	if (screen_pos.x < 0 || screen_pos.x > globals::screen_width || screen_pos.y < 0 || screen_pos.y >
		globals::screen_height)
		return false;

	return true;
}
