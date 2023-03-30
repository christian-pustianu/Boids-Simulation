#pragma once

#include "Model.hpp"
#include "../third_party/stb/include/stb_image.h"

/**
* @brief Creates a terrain mesh made of triangles with vertices, normals and material.
*
* @param heightmap The path for the heightmap used to create the terrain.
* @param transformMatrix The matrix used to transform the cone.
*
* @return The terrain's mesh object.
*/
SimpleMesh make_terrain(const char* heightmap, Mat44f transformMatrix = Identity44f);
