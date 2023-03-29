#pragma once

#include <vector>
#include "../math/vec2.hpp"
#include "../math/vec3.hpp"

#include <glad.h>

struct Vertex {
	Vec3f positions;
	Vec3f normals;
	Vec2f texCoords;
};

struct Material {
	Vec3f ambient = { 0.8f, 0.8f, 0.8f };
	Vec3f diffuse = { 0.2f, 0.2f, 0.2f };
	Vec3f specular = { 0.f, 0.f, 0.f };
	Vec3f emission = { 0.f, 0.f, 0.f };
	float shininess = 0.5f;
	float alpha = 1.f;
};

class Mesh {
public:
	std::vector<Vertex> vertices;

	Mesh(std::vector<Vertex> vertices) {
		this->vertices = vertices;
	};
};

class SimpleMesh : public Mesh {
public:
	Material material;

	SimpleMesh(std::vector<Vertex> vertices, Material material) : Mesh(vertices) {
		this->material = material;
	};
};

class MultiMaterialMesh : public Mesh {
	public:
	std::vector<Material> materials;
	std::vector<int> materialIndexes;

	MultiMaterialMesh(std::vector<Vertex> vertices, std::vector<Material> materials, std::vector<int> materialIndexes) : Mesh(vertices) {
		this->materials = materials;
		this->materialIndexes = materialIndexes;
	};
};

class TexturedMesh : public Mesh {
public:
	std::vector<GLuint> textures;

	TexturedMesh(std::vector<Vertex> vertices, std::vector<GLuint> textures) : Mesh(vertices) {
		this->textures = textures;
	};
};