#pragma once

#include <vector>

#include <cstdlib>

#include "Mesh.hpp"

#include "../math/vec3.hpp"
#include "../math/mat44.hpp"
#include "../math/mat33.hpp"

Mesh make_cone(
	bool aCapped = true,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);
