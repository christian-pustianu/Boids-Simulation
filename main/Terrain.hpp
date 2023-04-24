#pragma once

#include "Model.hpp"
#include "../third_party/stb/include/stb_image.h"

#include <unordered_map>

/**
* @brief Uses a heightmap to create a terrain mesh made of triangles with vertices, normals and material.
*
* @param heightmap - The path for the heightmap used to create the terrain.
* @param material - The material of the terrain.
* @param transformMatrix - The matrix used to transform the terrain.
*
* @return The terrain's model object.
*/
Model generate_terrain(const char* heightmap, Material material, Mat44f transformMatrix = Identity44f);
