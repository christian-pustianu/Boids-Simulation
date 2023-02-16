#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "Mesh.hpp"

struct RenderData
{
	std::vector<GLuint> VBO;
	GLuint VAO = 0;
};

class Render
{	
private:
	void cleanup() {
		for (GLuint vbo : data.VBO)
			glDeleteBuffers(1, &vbo);

		glDeleteVertexArrays(1, &data.VAO);
	};

public:
	RenderData data;
	Material mat;
	std::size_t vertexCount;
	
	Render(Mesh mesh) {
		data = setupRendering(mesh);
		vertexCount = mesh.vertices.size();
		mat = mesh.material;
	};

	~Render() {
		cleanup();
	};
	
	RenderData setupRendering(Mesh const&);
	void render(Shader shader);
};

