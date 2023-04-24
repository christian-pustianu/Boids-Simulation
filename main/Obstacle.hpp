#pragma once

#include "Model.hpp"
#include "../math/vec3.hpp"

/**
* @brief Representation of an obstacle in 3D space. Abstract class.
*/
class Obstacle
{
public:
	Model* model;
	Vec3f position;
	Mat44f model2world;

	/**
	* @brief Abstract constructor for an obstacle.
	* 
	* @param model - pointer to Model object used to represent obstacle.
	* @param position - Vec3f position of obstacle.
	* 
	*/
	Obstacle(Model* model, Vec3f position) {
		this->model = model;
		this->position = position;
		this->model2world = make_translation(position);
	};

	/**
	* @brief Abstract function for collision detection.
	*
	* @param - Vec3f object.
	* 
	* @return false as default.
	*/
	virtual bool isColliding(Vec3f) {
		return false;
	};
};

/**
* @brief Representation of a spherical obstacle inheriting from Obstacle.
*/
class SphereObstacle : public Obstacle
{
private:
	float radius;
public:

	/**
	* @brief Constructor for Sphere Obstacle with attributes inherited from Obstacle.
	* 
	* @param model - pointer to Model object used to represent a sphere.
	* @param position - Vec3f position of sphere.
	* @param radius - float radius of sphere.
	* 
	*/
	SphereObstacle(Model* model, Vec3f position, float radius) : Obstacle(model, position) {
	this->radius = radius;
	this->model2world = make_translation(position) * make_scaling({ radius, radius, radius });
	};

	/**
	* @brief Detects collision of sphere obstacle with another position.
	*
	* @param boidPosition - Vec3f position of boid.
	*
	* @return true for collision / false otherwise.
	*/
	bool isColliding(Vec3f boidPosition) override {
		return length(boidPosition - this->position) < this->radius + 2.f;
	};
};

/**
* @brief Representation of a box obstacle inheriting from Obstacle.
*/
class BoxObstacle : public Obstacle
{
private:
	Vec3f size; // size of the box from origin
	Vec3f corner1;
	Vec3f corner2;
public:
	/**
	* @brief Constructor for Axis-Aligned Bounding Box Obstacle with attributes inherited from Obstacle.
	* 
	* @param model - pointer to Model object used to represent a box.
	* @param position - Vec3f position of box.
	* @param size - Vec3f for scaling the box.
	* 
	*/
	BoxObstacle(Model* model, Vec3f position, Vec3f size) : Obstacle(model, position) {
		this->size = size;
		this->model2world = make_translation(position) * make_scaling(size);
		this->corner1 = this->position - this->size - Vec3f{ 5.f, 5.f, 5.f };
		this->corner2 = this->position + this->size + Vec3f{ 5.f, 5.f, 5.f };
	}

	/**
	* @brief Detects collision of AABB obstacle with another position.
	*
	* @param boidPosition - Vec3f position of boid.
	*
	* @return true for collision / false otherwise.
	*/
	bool isColliding(Vec3f boidPosition) override {
		return boidPosition.x >= this->corner1.x && boidPosition.x <= this->corner2.x &&
			boidPosition.y >= this->corner1.y && boidPosition.y <= this->corner2.y &&
			boidPosition.z >= this->corner1.z && boidPosition.z <= this->corner2.z;
	}
};
