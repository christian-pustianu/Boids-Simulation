#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "../math/mat33.hpp"
#include "../third_party/stb/include/stb_image.h"

constexpr float cubePositions[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};

/**
* @brief Representation of a cubemap used for skyboxes.
*/
class Cubemap {
private:
	unsigned int textureID = 0;
	unsigned int VBO = 0, VAO = 0;
public:
	/**
	* @brief Creates a cube map object by loading the texture data
	* and setting up rendering data.
	*
	* @param cubemap - An array of 6 strings containing the paths to the
	* texture files for the cube map.
	*/
	Cubemap(const char* cubemap[6]);
	~Cubemap() {};
	
	/**
	* @brief Handles the rendering of a cube map object with the specified shader.
	*
	* @param projection - The projection matrix.
	* @param world2camera - The world to camera matrix.
	* @param shaderProgram - The shader's program ID.
	*
	* @return void
	*/
	void render(Mat44f projection, Mat44f world2camera, unsigned int shaderProgram);
};
