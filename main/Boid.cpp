#include "Boid.hpp"

void Boid::updateDirection(float speed, float transition) {
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
	
	// If out of simulation bounds
	if (this->currentPosition.x < X_MIN) {
		this->currentPosition.x += X_RANGE;
	}
	else if (this->currentPosition.x > X_MAX) {
		this->currentPosition.x -= X_RANGE;
	}

	if (this->currentPosition.y < Y_MIN) {
		this->currentPosition.y += Y_RANGE;
	}
	else if (this->currentPosition.y > Y_MAX) {
		this->currentPosition.y -= Y_RANGE;
	}

	if (this->currentPosition.z < Z_MIN) {
		this->currentPosition.z += Z_RANGE;
	}
	else if (this->currentPosition.z > Z_MAX) {
		this->currentPosition.z -= Z_RANGE;
	}

	this->translationMatrix = make_translation(currentPosition);

	// Update model2world
	this->model2world = this->translationMatrix * this->rotationMatrix;
}