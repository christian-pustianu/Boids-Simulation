#pragma once

#include "Model.hpp"

/**
* @brief Creates a cone mesh made of triangles with vertices, normals and material.
* 
* @param subdivs The number of subdivisions of the cone.
* @param color The color of the cone (will be assigned to the material's ambient element).
* @param transformMatrix The matrix used to transform the cone.
* 
* @return The cone's model object.
*/
Model generate_cone(std::size_t subdivs = 16, Material material = {}, Mat44f transformMatrix = Identity44f);
