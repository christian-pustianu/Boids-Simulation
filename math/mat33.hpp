// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.

#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "mat44.hpp"

// Mat33f: 3x3 matrix with floats
struct Mat33f
{
	float v[9];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 3 && aJ < 3 );
		return v[aI*3 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 3 && aJ < 3 );
		return v[aI*3 + aJ];
	}
};

// Identity matrix
constexpr Mat33f kIdentity33f = { {
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
} };

// Common operators for Mat33f:
constexpr
Vec3f operator*( Mat33f const& leftSide, Vec3f const& rightSide ) noexcept
{
	Vec3f result{};
	for( std::size_t j = 0; j < 3; ++j )
	{
		for( std::size_t i = 0; i < 3; ++i )
		{
			result[j] += leftSide(j,i) * rightSide[i];
		}
	}
	return result;
}

// Functions:
inline
Mat33f mat44_to_mat33( Mat44f const& Matrix )
{
	Mat33f result{};
	for( std::size_t i = 0; i < 3; ++i )
	{
		for( std::size_t j = 0; j < 3; ++j )
			result(i,j) = Matrix(i,j);
	}
	return result;
}
