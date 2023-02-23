// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.

#pragma once

#include <vector>

#include <cstdlib>

#include "Mesh.hpp"

#include "../math/vec3.hpp"
#include "../math/mat44.hpp"
#include "../math/mat33.hpp"

Mesh make_cone(
	bool capped = true,
	std::size_t subdivs = 16,
	Vec3f color = { 1.f, 1.f, 1.f },
	Mat44f transformMatrix = Identity44f
);
