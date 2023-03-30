#pragma once

#include <glad.h>
#include "Shader.hpp"

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"
#include "../math/vec2.hpp"

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

struct Vertex {
	Vec3f positions;
	Vec3f normals;
	Vec2f texCoords;
};

struct Material {
	Vec3f ambient = { 0.8f, 0.8f, 0.8f };
	Vec3f diffuse = { 0.6f, 0.6f, 0.6f };
	Vec3f specular = { 0.2f, 0.2f, 0.2f };
	Vec3f emission = { 0.f, 0.f, 0.f };
	float shininess = 0.5f;
	float alpha = 1.f;
};

class Model {
public:
	std::vector<Vertex> vertices;
	std::vector<Material> materials;
	std::vector<unsigned int> materialIndexes;

	Mat44f model2world = Identity44f;

	Model(std::vector<Vertex> vertices, Material material) {
		this->vertices = vertices;
		this->materials = { material };
		setupRendering();
	}

	Model(std::vector<Vertex> vertices, std::vector<Material> materials, std::vector<unsigned int> materialIndexes) {
		this->vertices = vertices;
		this->materials = materials;
		this->materialIndexes = materialIndexes;
		setupRendering();
	}

	~Model() {};

	void cleanup() {
		for (GLuint vbo : VBO)
			glDeleteBuffers(1, &vbo);

		glDeleteVertexArrays(1, &VAO);
	};
	
	void setupRendering();
	void render(Vec3f, Mat44f, Mat44f, GLuint[]);

private:
	std::vector<GLuint> VBO;
	GLuint VAO;
};
