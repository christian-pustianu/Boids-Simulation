// Some code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.
// This only includes the struct definition and the operator definitions.
// The Functions section is my own work.

#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "other.hpp"

// Vec3f: 3D vector with floats
struct Vec3f {
	float x, y, z;

	constexpr
	float& operator[] (std::size_t aI) noexcept {
		assert( aI < 3 );
		return aI[&x];
	}
	constexpr
	float operator[] (std::size_t aI) const noexcept {
		assert( aI < 3 );
		return aI[&x];
	}
};

// Common operators for Vec3f:
constexpr
Vec3f operator+( Vec3f vector ) noexcept {
	return vector;
}

constexpr
Vec3f operator-( Vec3f vector ) noexcept {
	return { -vector.x, -vector.y, -vector.z };
}

constexpr
Vec3f operator+( Vec3f leftSide, Vec3f rightSide ) noexcept {
	return Vec3f {
		leftSide.x + rightSide.x,
		leftSide.y + rightSide.y,
		leftSide.z + rightSide.z
	};
}

constexpr
Vec3f operator-( Vec3f leftSide, Vec3f rightSide ) noexcept {
	return Vec3f {
		leftSide.x - rightSide.x,
		leftSide.y - rightSide.y,
		leftSide.z - rightSide.z
	};
}

constexpr
Vec3f operator*( float scalar, Vec3f vector ) noexcept {
	return Vec3f { 
		scalar * vector.x, 
		scalar * vector.y, 
		scalar * vector.z
	};
}

constexpr
Vec3f operator*( Vec3f vector, float scalar ) noexcept {
	return scalar * vector;
}

constexpr
Vec3f operator/( Vec3f vector, float scalar ) noexcept {
	return Vec3f {
		vector.x / scalar,
		vector.y / scalar,
		vector.z / scalar
	};
}

constexpr
Vec3f& operator+=( Vec3f& leftSide, Vec3f rightSide ) noexcept {
	leftSide.x += rightSide.x;
	leftSide.y += rightSide.y;
	leftSide.z += rightSide.z;
	return leftSide;
}

constexpr
Vec3f& operator-=( Vec3f& leftSide, Vec3f rightSide ) noexcept {
	leftSide.x -= rightSide.x;
	leftSide.y -= rightSide.y;
	leftSide.z -= rightSide.z;
	return leftSide;
}

constexpr
Vec3f& operator*=( Vec3f& leftSide, float scalar ) noexcept {
	leftSide.x *= scalar;
	leftSide.y *= scalar;
	leftSide.z *= scalar;
	return leftSide;
}

constexpr
Vec3f& operator/=( Vec3f& leftSide, float scalar ) noexcept {
	leftSide.x /= scalar;
	leftSide.y /= scalar;
	leftSide.z /= scalar;
	return leftSide;
}


// Functions:
// Dot product of two vectors
constexpr
float dot( Vec3f leftSide, Vec3f rightSide ) noexcept {
	return leftSide.x * rightSide.x 
		+ leftSide.y * rightSide.y
		+ leftSide.z * rightSide.z;
}

// Length of a vector
inline
float length( Vec3f vector ) noexcept {
	return std::sqrt( dot( vector, vector ) );
}

// Cross product of two vectors, return a vector perpendicular to both
inline
Vec3f cross(Vec3f leftSide, Vec3f rightSide) noexcept {
	return Vec3f {
		leftSide.y * rightSide.z - leftSide.z * rightSide.y,
		leftSide.z * rightSide.x - leftSide.x * rightSide.z,
		leftSide.x * rightSide.y - leftSide.y * rightSide.x
	};
}

// Normalize a vector, return the same vector if it's a null vector
inline
Vec3f normalize( Vec3f vector ) noexcept {
	if( vector.x == 0 && vector.y == 0 && vector.z == 0)
		return vector;

	auto const l = length( vector );
	return vector / l;
}

// Clamp a vector between two values
inline
Vec3f clamp_vec(Vec3f vector, float min, float max) noexcept {
	return Vec3f {
		clamp(vector.x, min, max),
		clamp(vector.y, min, max),
		clamp(vector.z, min, max)
	};
}

// Linear interpolation between two vectors
inline
Vec3f lerp(Vec3f vector1, Vec3f vector2, float weight) noexcept {
	return vector1 + weight * (vector2 - vector1);
}

// Spherical linear interpolation between two vectors
inline
Vec3f slerp(Vec3f vector1, Vec3f vector2, float weight) noexcept {
	float V1dotV2 = clamp(dot(vector1, vector2), -1.f, 1.f);
	float theta = std::acos(V1dotV2) * weight;
	Vec3f RelativeVec = normalize(vector2 - vector1 * V1dotV2);
	return (vector1 * std::cos(theta)) + (RelativeVec * std::sin(theta));
}

// Convert RGB color to linear space
inline
Vec3f rgb_to_linear(Vec3f color) noexcept {
	return Vec3f {
		color.x / 255.f,
		color.y / 255.f,
		color.z / 255.f
	};
}