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
	Vec3f ambient = { 1.f, 0.39216f, 0.39216f };
	Vec3f diffuse = { 0.2f, 0.2f, 0.2f };
	Vec3f specular = { 0.f, 0.f, 0.f };
	Vec3f emission = { 0.f, 0.f, 0.f };
	float shininess = 0.5f;
	float alpha = 1.f;
};

class Mesh
{
public:
	bool hasTexture = false;
	std::vector<Vertex> vertices;
	std::vector<GLuint> textures;
	Material material;

	Mesh(std::vector<Vertex> vertices, Material material) {
		this->vertices = vertices;
		this->material = material;
	};
		
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> textures = {}) {
		this->vertices = vertices;
		this->textures = textures;
		if (this->textures.size() != 0)
			this->hasTexture = true;
	};

};
