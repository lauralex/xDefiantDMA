#include "player.h"
#include "utils.h"


player::player(const uintptr_t address)
	: address_(address), temp_health_component_(NULL), health_component_(NULL), hp_(0), temp_animation_address_(NULL),
	  animation_address_(NULL), temp_bone_array_address_(NULL),
	  bone_array_address_(NULL), bone_array_size_unmasked_address_(NULL),
	  bone_address_unshifted_(NULL), bone_address_(NULL), bone_position_matrix_(), temp_team_id_address_(NULL),
	  team_id_(0)
{
}

player::player(const VMMDLL_SCATTER_HANDLE scatter, const int index, const uintptr_t decrypted_player_list):
	address_(NULL), temp_health_component_(NULL), health_component_(NULL), hp_(0),
	temp_animation_address_(NULL),
	animation_address_(NULL),
	temp_bone_array_address_(NULL),
	bone_array_address_(NULL),
	bone_array_size_unmasked_address_(NULL), bone_address_unshifted_(NULL),
	bone_address_(NULL), bone_position_matrix_(), temp_team_id_address_(NULL), team_id_(0)
{
	get_player_by_id_scatter(scatter, index, decrypted_player_list, address_);
}

void player::update_1(const VMMDLL_SCATTER_HANDLE scatter_handle)
{
	get_temp_animation_scatter(scatter_handle, address_, temp_animation_address_);
	get_temp_team_id_scatter(scatter_handle, address_, temp_team_id_address_);
	get_temp_health_component_scatter(scatter_handle, address_, temp_health_component_);
}

void player::update_2(const VMMDLL_SCATTER_HANDLE scatter_handle)
{
	animation_address_ = get_animation_from_temp(temp_animation_address_);
	get_temp_bone_array_address(scatter_handle, animation_address_, temp_bone_array_address_);
	get_team_id_scatter(scatter_handle, temp_team_id_address_, team_id_);
	health_component_ = get_health_component_from_temp(temp_health_component_);
	get_health_scatter(scatter_handle, health_component_, hp_);
}

void player::update_3(const VMMDLL_SCATTER_HANDLE scatter_handle)
{
	bone_array_address_ = get_bone_array_address_from_temp(temp_bone_array_address_);
	if (!is_memory_pointer_valid(bone_array_address_))
	{
		bone_array_address_ = mem.Read<uintptr_t>(animation_address_ + 0x40) + 0x40;
	}
	mem.AddScatterReadRequest(scatter_handle, bone_array_address_ + 0x58, &bone_array_size_unmasked_address_,
	                          sizeof(uintptr_t));
	mem.AddScatterReadRequest(scatter_handle, bone_array_address_ + 0x50, &bone_address_unshifted_, sizeof(uintptr_t));
}

void player::update_4()
{
	if (is_memory_pointer_valid(bone_array_size_unmasked_address_))
	{
		constexpr auto bone_index = 11;
		bone_address_ = get_bone_address_from_array(bone_array_address_, bone_address_unshifted_, bone_index,
		                                            bone_array_size_unmasked_address_);
	}
}

void player::update_5(const VMMDLL_SCATTER_HANDLE scatter_handle)
{
	get_bone_position_scatter(scatter_handle, bone_position_matrix_, bone_address_);
}

vector3_t player::get_player_position() const
{
	return vector3_t{bone_position_matrix_._41, bone_position_matrix_._42, bone_position_matrix_._43};
}

bool player::operator==(const player& other) const
{
	return address_ == other.address_;
}

bool player::operator==(const uintptr_t& other) const
{
	return address_ == other;
}

bool player::is_equal(const player& other) const
{
	return address_ == other.address_;
}

bool player::is_equal(const uintptr_t& other) const
{
	return address_ == other;
}

bool player::is_teammate(const player& other) const
{
	return team_id_ == other.team_id_;
}

bool player::is_teammate(const int& index) const
{
	return team_id_ == index;
}

bool player::is_dead() const
{
	return hp_ <= 0;
}
