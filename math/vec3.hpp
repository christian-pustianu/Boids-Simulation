// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.

#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

// Vec3f: 3D vector with floats
struct Vec3f
{
	float x, y, z;

	constexpr
	float& operator[] (std::size_t aI) noexcept
	{
		assert( aI < 3 );
		return aI[&x];
	}
	constexpr
	float operator[] (std::size_t aI) const noexcept
	{
		assert( aI < 3 );
		return aI[&x];
	}
};

// Common operators for Vec3f:
constexpr
Vec3f operator+( Vec3f vector ) noexcept
{
	return vector;
}

constexpr
Vec3f operator-( Vec3f vector ) noexcept
{
	return { -vector.x, -vector.y, -vector.z };
}

constexpr
Vec3f operator+( Vec3f leftSide, Vec3f rightSide ) noexcept
{
	return Vec3f{
		leftSide.x + rightSide.x,
		leftSide.y + rightSide.y,
		leftSide.z + rightSide.z
	};
}

constexpr
Vec3f operator-( Vec3f leftSide, Vec3f rightSide ) noexcept
{
	return Vec3f{
		leftSide.x - rightSide.x,
		leftSide.y - rightSide.y,
		leftSide.z - rightSide.z
	};
}

constexpr
Vec3f operator*( float scalar, Vec3f vector ) noexcept
{
	return Vec3f{ 
		scalar * vector.x, 
		scalar * vector.y, 
		scalar * vector.z
	};
}

constexpr
Vec3f operator*( Vec3f vector, float scalar ) noexcept
{
	return scalar * vector;
}

constexpr
Vec3f operator/( Vec3f vector, float scalar ) noexcept
{
	return Vec3f{ 
		vector.x / scalar,
		vector.y / scalar,
		vector.z / scalar
	};
}

constexpr
Vec3f& operator+=( Vec3f& leftSide, Vec3f rightSide ) noexcept
{
	leftSide.x += rightSide.x;
	leftSide.y += rightSide.y;
	leftSide.z += rightSide.z;
	return leftSide;
}

constexpr
Vec3f& operator-=( Vec3f& leftSide, Vec3f rightSide ) noexcept
{
	leftSide.x -= rightSide.x;
	leftSide.y -= rightSide.y;
	leftSide.z -= rightSide.z;
	return leftSide;
}

constexpr
Vec3f& operator*=( Vec3f& leftSide, float rightSide ) noexcept
{
	leftSide.x *= rightSide;
	leftSide.y *= rightSide;
	leftSide.z *= rightSide;
	return leftSide;
}

constexpr
Vec3f& operator/=( Vec3f& leftSide, float rightSide ) noexcept
{
	leftSide.x /= rightSide;
	leftSide.y /= rightSide;
	leftSide.z /= rightSide;
	return leftSide;
}


// Functions:
constexpr
float dot( Vec3f leftSide, Vec3f rightSide ) noexcept
{
	return leftSide.x * rightSide.x 
		+ leftSide.y * rightSide.y
		+ leftSide.z * rightSide.z
	;
}

inline
float length( Vec3f vector ) noexcept
{
	return std::sqrt( dot( vector, vector ) );
}

inline
Vec3f cross(Vec3f leftSide, Vec3f rightSide) noexcept
{
	return Vec3f{
		leftSide.y * rightSide.z - leftSide.z * rightSide.y,
		leftSide.z * rightSide.x - leftSide.x * rightSide.z,
		leftSide.x * rightSide.y - leftSide.y * rightSide.x
	};
}

inline
Vec3f normalize( Vec3f vector ) noexcept
{
	auto const l = length( vector );
	return vector / l;
}
