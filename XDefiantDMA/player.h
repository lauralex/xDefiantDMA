#pragma once
#include <cstdint>

#include "structs.h"
#include "Memory/Memory.h"

class player
{
public:
	player(uintptr_t address);
	player(VMMDLL_SCATTER_HANDLE scatter, int index, uintptr_t decrypted_player_list);
	void update_1(VMMDLL_SCATTER_HANDLE scatter_handle);
	void update_2(VMMDLL_SCATTER_HANDLE scatter_handle);
	void update_3(VMMDLL_SCATTER_HANDLE scatter_handle);
	void update_4();
	void update_5(VMMDLL_SCATTER_HANDLE scatter_handle);
	bool operator==(const player& other) const;
	bool operator==(const uintptr_t& other) const;
	[[nodiscard]] vector3_t get_player_position() const;
	[[nodiscard]] bool is_equal(const player& other) const;
	[[nodiscard]] bool is_equal(const uintptr_t& other) const;
	[[nodiscard]] bool is_teammate(const player& other) const;
	[[nodiscard]] bool is_teammate(const int& index) const;
	[[nodiscard]] bool is_dead() const;

private:
	uintptr_t address_;
	uintptr_t temp_health_component_;
	uintptr_t health_component_;
	int hp_;
	uintptr_t temp_animation_address_;
	uintptr_t animation_address_;
	uintptr_t temp_bone_array_address_;
	uintptr_t bone_array_address_;
	uintptr_t bone_array_size_unmasked_address_;
	uintptr_t bone_address_unshifted_;
	uintptr_t bone_address_;
	matrix4_t bone_position_matrix_;
	uintptr_t temp_team_id_address_;
	int team_id_;
};
