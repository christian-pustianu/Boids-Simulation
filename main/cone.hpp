#ifndef CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29
#define CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29

#include <vector>

#include <cstdlib>

#include "Mesh.h"

#include "../math/vec3.hpp"
#include "../math/mat44.hpp"
#include "../math/mat33.hpp"

Mesh make_cone(
	bool aCapped = true,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

#endif // CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29
