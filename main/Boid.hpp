#pragma once

#include "Model.hpp"
#include "../math/other.hpp"

class Boid : public Model
{
private:
	Vec3f initialDirection = { 1.f, 0.f, 0.f };
	Vec3f currentDirection = {};
	Vec3f targetDirection = {};

	Mat44f rotationMatrix = Identity44f;
	Mat44f translationMatrix = Identity44f;

	// give random position within simulation space
	void randomizePosition() {
		// spawn boid at random position within simulation space (x, y, z) = (-100, 2, -100) to (100, 50, 100)
		this->currentPosition = { (float)rand() / RAND_MAX * 200.f - 100.f, 
									(float)rand() / RAND_MAX * 48.f + 2.f, 
									(float)rand() / RAND_MAX * 200.f - 100.f };
	}

	// give random direction
	void randomizeDirection() {
		this->currentDirection = normalize(Vec3f{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX });
		//this->currentDirection = { 0.f, 0.f, 1.f };
	}

public:
	Vec3f currentPosition = { 0.f, 2.f, 0.f };

	Boid(Mesh mesh) : Model(mesh) {
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

	void updateDirection(float speed, float transition) {
		// Rotation on custom axis from object's original direction to currentDirection
		float rotationAngle = acos(dot(this->initialDirection, this->currentDirection));
		Vec3f rotationAxis = cross(this->initialDirection, this->currentDirection);
		this->rotationMatrix = make_rotation_custom_axis(normalize(rotationAxis), rotationAngle);

		// Translation using different types of linear interpolation

		// Compute the angle between the vectors
		float angle = degrees(acos(dot(this->currentDirection, this->targetDirection)));

		// if the angle is greater than 90 degrees, lerp does not work properly,
		// so we use slerp instead for the first 90 degrees
		if (angle >= 90.f) {
			if (angle >= 180.f) {
				// if the vectors are exactly opposite, slerp doesn't work, but it works if we 
				// add a very small value to the currentDirection to get it started
				this->currentDirection = this->currentDirection + Vec3f{ 0.00000001f, 0.f, 0.f };
			}
			this->currentDirection = slerp(this->currentDirection, this->targetDirection, transition);
		}
		else {
			this->currentDirection = normalize(lerp(this->currentDirection, this->targetDirection, transition));
		}
		this->currentPosition += this->currentDirection * speed;
		this->translationMatrix = make_translation(currentPosition);

		// Update model2world
		this->model2world = this->translationMatrix * this->rotationMatrix;
	}
};