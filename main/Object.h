#pragma once

#include <glad.h>
#include "Shader.h"

class Object
{
private:
	
public:
	// Container struct for VAOs and VBOs.
	struct RenderData
	{
		unsigned int VAO;
		unsigned int VBO;

		void cleanup() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};
	RenderData data;
	
	//Object(SimpleMeshData);
	Object() {
		data = setupRendering();
	};
	
	RenderData setupRendering();
	void render(Shader shader);
};

