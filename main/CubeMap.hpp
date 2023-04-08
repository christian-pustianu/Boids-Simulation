#pragma once

#include <glad.h>
#include "Shader.hpp"

/**
* @brief 
*
* @param heightmap - The path for the heightmap used to create the terrain.
* @param material - The material of the terrain.
* @param transformMatrix - The matrix used to transform the terrain.
*
* @return The terrain's mesh object.
*/
void renderCubeMap(const char* cubemap[6], Shader shader);