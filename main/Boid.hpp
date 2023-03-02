#pragma once

#include <vector>

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"
#include "../math/other.hpp"

constexpr float X_MIN = -100.f;
constexpr float X_MAX = 100.f;
constexpr float X_RANGE = X_MAX - X_MIN;

constexpr float Y_MIN = 1.f;
constexpr float Y_MAX = 50.f;
constexpr float Y_RANGE = Y_MAX - Y_MIN;

constexpr float Z_MIN = -100.f;
constexpr float Z_MAX = 100.f;
constexpr float Z_RANGE = Z_MAX - Z_MIN;

constexpr float EDGE_LIMIT = 2.f;

class Boid
{
private:
	Vec3f initialDirection = { 1.f, 0.f, 0.f };
	Vec3f targetDirection = {};

	Mat44f rotationMatrix = Identity44f;
	Mat44f translationMatrix = Identity44f;

	// give random position within simulation space
	void randomizePosition() {
		// spawn boid at random position within simulation space (x, y, z) = (-100, 2, -100) to (100, 50, 100)
		this->currentPosition = { (float)rand() / RAND_MAX * X_RANGE + X_MIN,
									(float)rand() / RAND_MAX * Y_RANGE + Y_MIN,
									(float)rand() / RAND_MAX * Z_RANGE + Z_MIN };
	}

	// give random direction
	void randomizeDirection() {
		this->currentDirection = normalize(Vec3f{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX });
	}

public:
	Vec3f currentDirection = {};
	Vec3f currentPosition = { 0.f, 2.f, 0.f };
	Mat44f model2world = Identity44f;

	Boid() {
		// Spawn boid at random position and direction, going straight forward
		randomizePosition();
		randomizeDirection();
		this->targetDirection = currentDirection;
		// Initial rotation for object
		float rotationAngle = acos(dot(this->initialDirection, this->currentDirection));
		Vec3f rotationAxis = cross(this->initialDirection, this->currentDirection);
		this->rotationMatrix = make_rotation_custom_axis(rotationAxis, rotationAngle);

		this->translationMatrix = make_translation(currentPosition);
		this->model2world = this->translationMatrix * this->rotationMatrix;
	};

	void setTargetDirection(Vec3f direction) {
		targetDirection = direction;
	}

	void updateDirection(float, float);

	std::vector<Boid*> findNeighbours(std::vector<Boid*>, float);

	Vec3f applyCohesion(std::vector<Boid*>, float);

	Vec3f applyAlignment(std::vector<Boid*>, float);

	Vec3f applySeparation(std::vector<Boid*>, float, float);

	Vec3f avoidEdges(float);
};