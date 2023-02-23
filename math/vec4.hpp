// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.

#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

// Vec2f: 4D vector with floats
struct Vec4f
{
	float x, y, z, w;

	constexpr 
	float& operator[] (std::size_t aI) noexcept
	{
		assert( aI < 4 );
		return aI[&x];
	}
	constexpr 
	float operator[] (std::size_t aI) const noexcept
	{
		assert( aI < 4 );
		return aI[&x];
	}
};

// Common operators for Vec4f:
constexpr
Vec4f operator+( Vec4f vector ) noexcept
{
	return vector;
}
constexpr
Vec4f operator-( Vec4f vector ) noexcept
{
	return { -vector.x, -vector.y, -vector.z, -vector.w };
}

constexpr
Vec4f operator+( Vec4f leftSide, Vec4f rightSide ) noexcept
{
	return Vec4f{
		leftSide.x + rightSide.x,
		leftSide.y + rightSide.y,
		leftSide.z + rightSide.z,
		leftSide.w + rightSide.w
	};
}
constexpr
Vec4f operator-( Vec4f leftSide, Vec4f rightSide ) noexcept
{
	return Vec4f{
		leftSide.x - rightSide.x,
		leftSide.y - rightSide.y,
		leftSide.z - rightSide.z,
		leftSide.w - rightSide.w
	};
}


constexpr
Vec4f operator*( float scalar, Vec4f vector ) noexcept
{
	return Vec4f{ 
		scalar * vector.x, 
		scalar * vector.y, 
		scalar * vector.z, 
		scalar * vector.w 
	};
}
constexpr
Vec4f operator*( Vec4f vector, float scalar ) noexcept
{
	return scalar * vector;
}

constexpr
Vec4f operator/( Vec4f vector, float scalar ) noexcept
{
	return Vec4f{ 
		vector.x / scalar,
		vector.y / scalar,
		vector.z / scalar,
		vector.w / scalar
	};
}


constexpr
Vec4f& operator+=( Vec4f& leftSide, Vec4f rightSide ) noexcept
{
	leftSide.x += rightSide.x;
	leftSide.y += rightSide.y;
	leftSide.z += rightSide.z;
	leftSide.w += rightSide.w;
	return leftSide;
}
constexpr
Vec4f& operator-=( Vec4f& leftSide, Vec4f rightSide ) noexcept
{
	leftSide.x -= rightSide.x;
	leftSide.y -= rightSide.y;
	leftSide.z -= rightSide.z;
	leftSide.w -= rightSide.w;
	return leftSide;
}

constexpr
Vec4f& operator*=( Vec4f& leftSide, float rightSide ) noexcept
{
	leftSide.x *= rightSide;
	leftSide.y *= rightSide;
	leftSide.z *= rightSide;
	leftSide.w *= rightSide;
	return leftSide;
}
constexpr
Vec4f& operator/=( Vec4f& leftSide, float rightSide ) noexcept
{
	leftSide.x /= rightSide;
	leftSide.y /= rightSide;
	leftSide.z /= rightSide;
	leftSide.w /= rightSide;
	return leftSide;
}


// Functions:
constexpr
float dot( Vec4f leftSide, Vec4f rightSide ) noexcept
{
	return leftSide.x * rightSide.x 
		+ leftSide.y * rightSide.y
		+ leftSide.z * rightSide.z
		+ leftSide.w * rightSide.w;
}

inline
float length( Vec4f vector ) noexcept
{
	return std::sqrt( dot( vector, vector ) );
}
