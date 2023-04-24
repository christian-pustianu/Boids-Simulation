#pragma once

#include "Model.hpp"

/**
* @brief Uses rapidobj and .obj files to create a mesh made of triangles with vertices, normals and material.
*
* @param objPath - The path for the .obj file.
* 
* @return The mesh's model object.
*/
Model load_wavefront_obj( char const* objPath);
