#pragma once
#include <math.h>

// Struct of float vector3
struct vector3_t
{
	float x, y, z;

	[[nodiscard]] float distance(const vector3_t& other) const
	{
		const auto x_d = x - other.x;
		const auto y_d = y - other.y;
		const auto z_d = z - other.z;
		return sqrtf(x_d * x_d + y_d * y_d + z_d * z_d);
	}
};

// Struct of float vector2
struct vector2_t
{
	float x, y;

	[[nodiscard]] float distance(const vector2_t& other) const
	{
		const auto x_d = x - other.x;
		const auto y_d = y - other.y;
		return sqrtf(x_d * x_d + y_d * y_d);
	}
};

// Struct of float matrix4
union matrix4_t
{
	struct
	{
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;
	};

	float m[4][4];
};
