#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "Mesh.hpp"

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"

#include <cstdlib>
#include <ctime>

struct RenderData
{
	std::vector<GLuint> VBO;
	GLuint VAO = 0;
};

class Model
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

	Mat44f model2world = Identity44f;
	
	Model(Mesh mesh) {
		data = setupRendering(mesh);
		vertexCount = mesh.vertices.size();
		mat = mesh.material;
	};

	~Model() {
		cleanup();
	};
	
	RenderData setupRendering(Mesh const&);
	void render(Shader shader);
};
