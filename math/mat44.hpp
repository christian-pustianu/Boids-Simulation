// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.
// This only includes the struct and the operator definitions.
// The Functions section is my own work.

#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

// Mat44f: 4x4 matrix with floats
struct Mat44f
{
	float v[16];

	constexpr
		float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert(aI < 4 && aJ < 4);
		return v[aI * 4 + aJ];
	}
	constexpr
		float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert(aI < 4 && aJ < 4);
		return v[aI * 4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f Identity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f:
constexpr
Mat44f operator*(Mat44f const& leftSide, Mat44f const& rightSide) noexcept
{
	Mat44f result = {};
	float sum = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			sum = 0;
			for (int k = 0; k < 4; k++) {
				sum += leftSide(i, k) * rightSide(k, j);
			}
			result(i, j) = sum;
		}
	}
	return result;
}

constexpr
Vec4f operator*(Mat44f const& leftSide, Vec4f const& rightSide) noexcept
{
	Vec4f result = {};
	float sum = 0;
	for (int i = 0; i < 4; i++) {
		sum = 0;
		for (int k = 0; k < 4; k++) {
			sum += leftSide(i, k) * rightSide[k];
		}
		result[i] = sum;
	}
	return result;
}

constexpr
Mat44f operator*=(Mat44f& leftSide, Mat44f const& rightSide) noexcept
{
	Mat44f result = {};
	float sum = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			sum = 0;
			for (int k = 0; k < 4; k++) {
				sum += leftSide(i, k) * rightSide(k, j);
			}
			result(i, j) = sum;
		}
	}
	for (int i = 0; i < 16; i++) {
		leftSide.v[i] = result.v[i];
	}
	return leftSide;
}

Mat44f invert(Mat44f const& Matrix) noexcept;

// Functions:
// Transposes a matrix
inline
Mat44f transpose(Mat44f const& Matrix) noexcept
{
	Mat44f result;
	for (std::size_t i = 0; i < 4; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
			result(j, i) = Matrix(i, j);
	}
	return result;
}

// Creates a lookAt matrix for a camera at a given position,
// looking at a given point, with a given up vector
inline
Mat44f look_at(Vec3f eye, Vec3f front, Vec3f up) noexcept
{
	Vec3f x, y, z;
	z = normalize(-front);
	y = up;
	x = cross(y, z);
	y = cross(z, x);
	x = normalize(x);
	y = normalize(y);

	return Mat44f{ x.x, x.y, x.z, -dot(x, eye),
				   y.x, y.y, y.z, -dot(y, eye),
				   z.x, z.y, z.z, -dot(z, eye),
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a rotation matrix for a given angle around an arbitrary axis
inline
Mat44f make_rotation_custom_axis(Vec3f axis, float angle) noexcept
{
	float c = cos(angle);
	float s = sin(angle);
	float t = 1 - c;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	return Mat44f{ t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0.f,
					t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0.f,
					t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0.f,
					0.f, 0.f, 0.f, 1.f };
}

// Creates a rotation matrix for a given angle around the X axis
inline
Mat44f make_rotation_x(float angle) noexcept
{
	return Mat44f{ 1.f, 0.f, 0.f, 0.f,
				   0.f, (float)cos(angle), (float)-sin(angle), 0.f,
				   0.f, (float)sin(angle), (float)cos(angle), 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a rotation matrix for a given angle around the Y axis
inline
Mat44f make_rotation_y(float angle) noexcept
{
	return Mat44f{ (float)cos(angle), 0.f, (float)sin(angle), 0.f,
				   0.f, 1.f, 0.f, 0.f,
				   (float)-sin(angle), 0.f, (float)cos(angle), 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a rotation matrix for a given angle around the Z axis
inline
Mat44f make_rotation_z(float angle) noexcept
{
	return Mat44f{ (float)cos(angle), (float)-sin(angle), 0.f, 0.f,
				   (float)sin(angle), (float)cos(angle), 0.f, 0.f,
				   0.f, 0.f, 1.f, 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a shear matrix on the X axis
inline
Mat44f make_shear_x(float shearY, float shearZ) noexcept
{
	return Mat44f{ 1.f, 0.f, 0.f, 0.f,
				   shearY, 1.f, 0.f, 0.f,
				   shearZ, 0.f, 1.f, 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a shear matrix on the Y axis
inline
Mat44f make_shear_y(float shearX, float shearZ) noexcept
{
	return Mat44f{ 1.f, shearX, 0.f, 0.f,
				   0.f, 1.f, 0.f, 0.f,
				   0.f, shearZ, 1.f, 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a shear matrix on the Z axis
inline
Mat44f make_shear_z(float shearX, float shearY) noexcept
{
	return Mat44f{ 1.f, 0.f, shearX, 0.f,
				   0.f, 1.f, shearY, 0.f,
				   0.f, 0.f, 1.f, 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a translation matrix for a given translation vector
inline
Mat44f make_translation(Vec3f translation) noexcept
{
	return Mat44f{ 1.f, 0.f, 0.f, translation.x,
				   0.f, 1.f, 0.f, translation.y,
				   0.f, 0.f, 1.f, translation.z,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a scaling matrix for a given scaling vector
inline
Mat44f make_scaling(Vec3f scale) noexcept
{
	return Mat44f{ scale.x, 0.f, 0.f, 0.f,
				   0.f, scale.y, 0.f, 0.f,
				   0.f, 0.f, scale.z, 0.f,
				   0.f, 0.f, 0.f, 1.f };
}

// Creates a perspective projection matrix
inline
Mat44f make_perspective_projection(float fovInRadians, float aspectRatio, float nearPlane, float farPlane) noexcept
{
	float s = 1.f / (float)tan(fovInRadians / 2.f);
	float sx = s / aspectRatio;
	float sy = s;
	float a = -(farPlane + nearPlane) / (farPlane - nearPlane);
	float b = -2 * farPlane * nearPlane / (farPlane - nearPlane);
	return Mat44f{ sx, 0.f, 0.f, 0.f,
				   0.f, sy, 0.f, 0.f,
				   0.f, 0.f, a, b,
				   0.f, 0.f, -1.f, 1.f };
}
