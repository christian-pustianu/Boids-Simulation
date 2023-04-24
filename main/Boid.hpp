#pragma once

#include <vector>

#include "Obstacle.hpp"

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"
#include "../math/other.hpp"

constexpr float EDGE_LIMIT = 2.f;

constexpr float X_MIN = -100.f + EDGE_LIMIT;
constexpr float X_MAX = 100.f - EDGE_LIMIT;
constexpr float X_RANGE = X_MAX - X_MIN;

constexpr float Y_MIN = 0.f + EDGE_LIMIT;
constexpr float Y_MAX = 50.f - EDGE_LIMIT;
constexpr float Y_RANGE = Y_MAX - Y_MIN;

constexpr float Z_MIN = -100.f + EDGE_LIMIT;
constexpr float Z_MAX = 100.f - EDGE_LIMIT;
constexpr float Z_RANGE = Z_MAX - Z_MIN;


/**
 * @brief Abstract representation of a boid in the simulation space.
 * Needs a model to be rendered. Can be updated to move in the simulation space
 * using the model2world matrix.
 */
class Boid
{
private:
	Vec3f initialDirection = { 1.f, 0.f, 0.f };
	Vec3f targetDirection = {};

	Mat44f rotationMatrix = Identity44f;
	Mat44f translationMatrix = Identity44f;

	/**
	* @brief Sets the boid's currentPosition to a random one in the simulation space
	* that doesn't create collisions with obstacles.
	* 
	* @param obstacles - The obstacles in the simulation space.
	*
	* @return void
	*/
	void randomizePosition(std::vector<Obstacle*> &obstacles) {
		// spawn boid at random position within simulation space (x, y, z) = (-100, 2, -100) to (100, 50, 100)
		bool collision = false;
		do {
			collision = false;
			this->currentPosition = { (float)rand() / RAND_MAX * X_RANGE + X_MIN,
									(float)rand() / RAND_MAX * Y_RANGE + Y_MIN,
									(float)rand() / RAND_MAX * Z_RANGE + Z_MIN };
			for (auto obs : obstacles) {
				if (obs->isColliding(this->currentPosition)) {
					collision = true;
					break;
				}
			}
		} while (collision);
	}

	/**
	* @brief Sets the boid's currentDirection to a random direction.
	*
	* @return void
	*/
	void randomizeDirection() {
		this->currentDirection = normalize(Vec3f{ (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX });
	}

public:
	Vec3f currentDirection = {};
	Vec3f currentPosition = {};
	Mat44f model2world = Identity44f;

	/**
	* @brief Constructor - creates a Boid object at a random position facing a random direction.
	*
	* @param obstacles - A reference to a vector of pointers to obstacles, that is used in randomizePosition(obstacles).
	* 
	*/
	Boid(std::vector<Obstacle*> &obstacles) {
		// Spawn boid at random position and direction
		randomizePosition(obstacles);
		randomizeDirection();
		this->targetDirection = currentDirection;
		// Initial rotation for object
		float rotationAngle = acos(dot(this->initialDirection, this->currentDirection));
		Vec3f rotationAxis = normalize(cross(this->initialDirection, this->currentDirection));
		if (rotationAngle <= radians(90)) {
			this->rotationMatrix = make_rotation_custom_axis(rotationAxis, rotationAngle);
		}
		// If the angle is greater than 90 deg, we need to reverse the rotation to avoid the fish swimming upside down due to the normal rotation.
		else {
			this->rotationMatrix = make_rotation_custom_axis(rotationAxis, -(radians(180) - rotationAngle)) * make_rotation_y(radians(180));
		}
		// Initial translation for object
		this->translationMatrix = make_translation(currentPosition);
		this->model2world = this->translationMatrix * this->rotationMatrix;
	};

	/**
	* @brief Setter for targetDirection.
	*
	* @param direction - Given direction to set targetDirection to.
	*
	* @return void
	*/
	void setTargetDirection(Vec3f direction) {
		targetDirection = direction;
	}

	/**
	* @brief Updates the boid's currentDirection to be closer to the targetDirection 
	* using different types of linear interpolation and updates the boid's currentPosition
	* according to the new currentDirection.
	*
	* @param speed - The speed of the boid's movement.
	* @param transition - The weight of the interpolation.
	*
	* @return void
	*/
	void updateDirection(float, float);

	/**
	* @brief Finds all the boids within a given radius.
	*
	* @param totalBoids - A vector of pointers to all the boids in the simulation.
	* @param radius - The radius in which to search for neighbours.
	* @param visionAngle - The angle from the boid's current direction in which to search for neighbours.
	*
	* @return std::vector<Boid*> A vector of pointers to the neighbouring boids.
	*/
	std::vector<Boid*> findNeighbours(std::vector<Boid*>, float, float);

	/**
	* @brief Creates a direction vector towards the centre of mass of the neighbouring boids.
	*
	* @param neighbours - A vector of pointers to the neighbouring boids.
	* @param strength - The strength of the cohesion rule.
	*
	* @return Vec3f The direction vector created by the cohesion rule.
	*/
	Vec3f applyCohesion(std::vector<Boid*>, float);

	/**
	* @brief Creates a direction vector towards the average direction of the neighbouring boids.
	*
	* @param neighbours - A vector of pointers to the neighbouring boids.
	* @param strength - The strength of the alignment rule.
	*
	* @return Vec3f The direction vector created by the alignment rule.
	*/
	Vec3f applyAlignment(std::vector<Boid*>, float);

	/**
	* @brief Creates a direction vector away from the neighbouring boids.
	*
	* @param neighbours - A vector of pointers to the neighbouring boids.
	* @param strength - The strength of	the separation rule.
	*
	* @return Vec3f The direction vector created by the separation rule.
	*/
	Vec3f applySeparation(std::vector<Boid*>, float, float);

	/**
	* @brief Creates a direction vector away from the edges of the simulation space.
	*
	* @param strength - The strength of the rule.
	*
	* @return Vec3f The direction vector created by the rule.
	*/
	Vec3f avoidEdges(float);

	/**
	* @brief Checks for collisions with the obstacles in the simulation space
	* and creates a direction vector away from them.
	*
	* @param obstacles - A vector of pointers to the obstacles in the simulation space.
	* @param strength - The strength of the rule.
	*
	* @return Vec3f The direction vector created by the rule.
	*/
	Vec3f avoidObstacles(std::vector<Obstacle*>, float);
};