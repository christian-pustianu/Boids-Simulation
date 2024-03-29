// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.
// This only includes the struct and the operator definitions.
// The Functions section is my own work.

#pragma once

#include <cmath>

// Vec2f: 2D vector with floats
struct Vec2f
{
	float x, y;
};


// Common operators for Vec2f:
constexpr
Vec2f operator+( Vec2f vector ) noexcept
{
	return vector;
}

constexpr
Vec2f operator-( Vec2f vector ) noexcept
{
	return { -vector.x, -vector.y };
}

constexpr
Vec2f operator+( Vec2f leftSide, Vec2f rightSide ) noexcept
{
	return Vec2f{
		leftSide.x + rightSide.x,
		leftSide.y + rightSide.y
	};
}

constexpr
Vec2f operator-( Vec2f leftSide, Vec2f rightSide ) noexcept
{
	return Vec2f{
		leftSide.x - rightSide.x,
		leftSide.y - rightSide.y
	};
}

constexpr
Vec2f operator*( float aScalar, Vec2f vector ) noexcept
{
	return Vec2f{ aScalar * vector.x, aScalar * vector.y };
}

constexpr
Vec2f operator*( Vec2f vector, float aScalar ) noexcept
{
	return Vec2f{ vector.x * aScalar, vector.y * aScalar };
}

constexpr
Vec2f operator/( Vec2f vector, float aScalar ) noexcept
{
	return Vec2f{ vector.x / aScalar, vector.y / aScalar };
}

constexpr
Vec2f& operator+=( Vec2f& leftSide, Vec2f rightSide ) noexcept
{
	leftSide.x += rightSide.x;
	leftSide.y += rightSide.y;
	return leftSide;
}

constexpr
Vec2f& operator-=( Vec2f& leftSide, Vec2f rightSide ) noexcept
{
	leftSide.x -= rightSide.x;
	leftSide.y -= rightSide.y;
	return leftSide;
}

constexpr
Vec2f& operator*=( Vec2f& leftSide, float rightSide ) noexcept
{
	leftSide.x *= rightSide;
	leftSide.y *= rightSide;
	return leftSide;
}

constexpr
Vec2f& operator/=( Vec2f& leftSide, float rightSide ) noexcept
{
	leftSide.x /= rightSide;
	leftSide.y /= rightSide;
	return leftSide;
}


// Functions:
// Returns the dot product of two vectors
constexpr
float dot( Vec2f leftSide, Vec2f rightSide ) noexcept
{
	return leftSide.x * rightSide.x + leftSide.y * rightSide.y;
}

// Returns the length of a vector
inline
float length( Vec2f vector ) noexcept
{
	return std::sqrt( dot( vector, vector ) );
}

