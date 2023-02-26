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

class Boid : public Model
{
private:

	// give random position within simulation space
	void randomizePosition() {
		//this->model2world = make_translation(Vec3f{ (float)rand() / (float) (RAND_MAX / (200.f)), 
		//	(float)rand() / (float) (RAND_MAX / (50.f)),
		//	(float)rand() / (float) (RAND_MAX / (200.f)) });
		this->model2world = Identity44f;
	}

	// give random direction
	void randomizeDirection() {
		//srand(static_cast <unsigned> (time(0)));
		//this->currentDirection = normalize(Vec3f{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX });
		this->currentDirection = normalize(Vec3f{ 0.f, 0.f, 1.f });
	}

public:
	// ==== make private after debugging ====
	Vec3f currentDirection = {};
	Vec3f targetDirection = {};
	// ======================================
	Boid(Mesh mesh) : Model(mesh) {
		randomizePosition();
		randomizeDirection();
		this->targetDirection = currentDirection;
	};

	void setTargetDirection(Vec3f direction) {
		targetDirection = direction;
	}

	void updateDirection(float speed, float transition) {
		// Rotation
		float angle = acos(dot(this->currentDirection, this->targetDirection));
		Vec3f axis = cross(this->currentDirection, this->targetDirection);
		this->model2world *= make_rotation_from_axis_and_angle(axis, angle * speed);

		// Translation
		this->currentDirection = lerp(this->currentDirection, this->targetDirection, transition);
		this->model2world *= make_translation(currentDirection * speed);
	}
};