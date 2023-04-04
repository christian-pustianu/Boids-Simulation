#include "Boid.hpp"

void Boid::updateDirection(float speed, float transition) {
	// Rotation on custom axis from object's original direction to currentDirection
	float rotationAngle = acos(dot(this->initialDirection, this->currentDirection));
	Vec3f rotationAxis = cross(this->initialDirection, this->currentDirection);
	if (rotationAngle <= radians(90)) {
		this->rotationMatrix = make_rotation_custom_axis(normalize(rotationAxis), rotationAngle);
	}
	// If the angle is greater than 90 deg, we need to reverse the rotation to avoid the fish swimming upside down due to the normal rotation.
	else {
		this->rotationMatrix = make_rotation_custom_axis(normalize(rotationAxis), -(radians(180) - rotationAngle)) * make_rotation_y(radians(180));
	}

	// Translation using different types of linear interpolation

	// Compute the angle between the vectors
	float angle = degrees(acos(dot(this->currentDirection, this->targetDirection)));

	// If the angle is greater than 90 degrees, lerp does not work properly,
	// so we use slerp instead for the first 90 degrees
	if (angle >= 90.f) {
		if (angle >= 180.f) {
			// If the vectors are exactly opposite, slerp doesn't work, but it works if we 
			// add a very small value to the currentDirection to get the motion started
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

std::vector<Boid*> Boid::findNeighbours(std::vector<Boid*> totalBoids, float radius, float visionAngle) {
	std::vector<Boid*> neighbours;
	float distance;
	for (auto b : totalBoids) {
		Vec3f diff = b->currentPosition - this->currentPosition;
		distance = length(diff);
		if (distance > 0 && distance < radius) {
			float angle = acos(dot(this->currentDirection, diff));
			if (angle < visionAngle) {
				neighbours.push_back(b);
			}
		}
	}
	return neighbours;
}

Vec3f Boid::applyCohesion(std::vector<Boid*> neighbours, float strength) {
	if (neighbours.size() == 0) {
		return Vec3f{ 0.f, 0.f, 0.f };
	}

	Vec3f cohesion = Vec3f{ 0.f, 0.f, 0.f };
	for (Boid* b : neighbours) {
		cohesion += b->currentPosition;
	}
	cohesion /= neighbours.size();
	cohesion -= this->currentPosition;
	return normalize(cohesion) * strength;
}

Vec3f Boid::applyAlignment(std::vector<Boid*> neighbours, float strength) {
	if (neighbours.size() == 0) {
		return Vec3f{ 0.f, 0.f, 0.f };
	}

	Vec3f alignment = Vec3f{ 0.f, 0.f, 0.f };
	for (Boid* b : neighbours) {
		alignment += b->currentDirection;
	}
	alignment /= neighbours.size();
	return normalize(alignment) * strength;
}

Vec3f Boid::applySeparation(std::vector<Boid*> neighbours, float strength, float radius) {
	if (neighbours.size() == 0) {
		return Vec3f{ 0.f, 0.f, 0.f };
	}

	std::vector<Boid*> closeNeighbours;
	for (Boid* b : neighbours) {
		if (length(b->currentPosition - this->currentPosition) < radius / 2) {
			closeNeighbours.push_back(b);
		}
	}

	if (closeNeighbours.size() == 0) {
		return Vec3f{ 0.f, 0.f, 0.f };
	}

	Vec3f separation = Vec3f{ 0.f, 0.f, 0.f };

	for (Boid* b : closeNeighbours) {
		separation += this->currentPosition - b->currentPosition;
	}
	separation /= closeNeighbours.size();
	return normalize(separation) * strength;
}

// turns back when reaching the edge of the simulation
Vec3f Boid::avoidEdges(float strength) {
	Vec3f direction = Vec3f{ 0.f, 0.f, 0.f };
	if (this->currentPosition.x < X_MIN) {
		direction.x += 1.f;
	}
	else if (this->currentPosition.x > X_MAX) {
		direction.x -= 1.f;
	}

	if (this->currentPosition.y < Y_MIN) {
		direction.y += 1.f;
	}
	else if (this->currentPosition.y > Y_MAX) {
		direction.y -= 1.f;
	}

	if (this->currentPosition.z < Z_MIN) {
		direction.z += 1.f;
	}
	else if (this->currentPosition.z > Z_MAX) {
		direction.z -= 1.f;
	}
	return direction * strength;
}

Vec3f Boid::avoidObstacles(std::vector<Obstacle*> obstacles, float strength) {
	Vec3f direction = Vec3f{ 0.f, 0.f, 0.f };
	for (auto o : obstacles) {
		if (o->isColliding(this->currentPosition)) {
			direction += this->currentPosition - o->position;
		}
	}
	return normalize(direction) * strength;
}