#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "../math/mat33.hpp"
#include "../third_party/stb/include/stb_image.h"


class CubeMap {
private:
	unsigned int textureID = 0;
	unsigned int VBO = 0, VAO = 0;
public:
	/**
	* @brief Creates a cube map object by loading the texture data
	* and setting up rendering data.
	*
	* @param shader - The shader to use for rendering the cube map.
	*/
	CubeMap(const char* cubemap[6]);
	~CubeMap() {};
	
	/**
	* @brief Handles the rendering of a cube map object with the specified shader.
	*
	* @param shader - The shader to use for rendering the cube map.
	*
	* @return void
	*/
	void render(Mat44f projection, Mat44f world2camera, unsigned int shaderProgram);
};
