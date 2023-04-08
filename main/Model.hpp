#pragma once

#include <glad.h>
#include "Shader.hpp"

#include "../math/mat44.hpp"
#include "../math/mat33.hpp"
#include "../math/vec3.hpp"
#include "../math/vec2.hpp"

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

/**
* @brief A vertex struct containing the parameters of a vertex.
*/
struct Vertex {
	Vec3f positions;
	Vec3f normals;
	Vec2f texCoords;
};

/**
* @brief A material struct containing the parameters of a material used in the Blinn-Phong model.
*/
struct Material {
	Vec3f ambient = { 0.8f, 0.8f, 0.8f };
	Vec3f diffuse = { 0.6f, 0.6f, 0.6f };
	Vec3f specular = { 0.2f, 0.2f, 0.2f };
	Vec3f emission = { 0.f, 0.f, 0.f };
	float shininess = 0.5f;
	float alpha = 1.f;
};

/**
* @brief A light struct containing the parameters of the light used in the Blinn-Phong model.
*/
struct Light {
	Vec3f position;
	Vec3f ambient;
	Vec3f color;
	float strength;
};

/**
* @brief Representation of a 3D model.
*/
class Model {
private:
	std::vector<GLuint> VBO;
	GLuint VAO;

	/**
	* @brief Deletes the VBOs and VAO buffers.
	* 
	* @returns void
	*/
	void cleanup() {
		for (GLuint vbo : VBO)
			glDeleteBuffers(1, &vbo);

		glDeleteVertexArrays(1, &VAO);
	};

public:
	std::vector<Vertex> vertices;
	std::vector<Material> materials;
	std::vector<unsigned int> materialIndexes;

	Mat44f model2world = Identity44f;

	/**
	* @brief Constructor for a model with a single material.
	*/
	Model(std::vector<Vertex> vertices, Material material) {
		this->vertices = vertices;
		this->materials = { material };
		setupRendering();
	}

	/**
	* @brief Constructor for a model with multiple materials.
	*/
	Model(std::vector<Vertex> vertices, std::vector<Material> materials, std::vector<unsigned int> materialIndexes) {
		this->vertices = vertices;
		this->materials = materials;
		this->materialIndexes = materialIndexes;
		setupRendering();
	}

	~Model() {
		cleanup();
	};
	
	/**
	* @brief Sets up the VBOs and VAOs for rendering this model.
	*
	* @return void
	*/
	void setupRendering();

	/**
	* @brief Renders either a single material or a multi material model using the given shader programs.
	*
	* @param cameraPosition - The current camera position in world coordinates.
	* @param world2projection - The product of the projection and world2camera matrices.
	* @param model2world - The model's transformation matrix.
	* @param shaderProgs - Array of 2 shaders: 0 - singlematerial, 1 - multimaterial
	*
	* @return void
	*/
	void render(Vec3f, Light, Mat44f, Mat44f, GLuint[]);
};
