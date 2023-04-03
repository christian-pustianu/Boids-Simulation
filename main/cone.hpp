#pragma once

#include <vector>

#include <cstdlib>

#include "Model.hpp"


/**
* @brief Creates a cone mesh made of triangles with vertices, normals and material.
* 
* @param subdivs The number of subdivisions of the cone.
* @param color The color of the cone (will be assigned to the material's ambient element).
* @param transformMatrix The matrix used to transform the cone.
* 
* @return The cone's mesh object.
*/
Model make_cone(std::size_t subdivs = 16, Vec3f color = { 1.f, 1.f, 1.f }, Mat44f transformMatrix = Identity44f);
