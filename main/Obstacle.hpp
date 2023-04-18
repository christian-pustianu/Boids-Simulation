#pragma once

#include "Model.hpp"
#include "../math/vec3.hpp"

class Obstacle
{
public:
	Model* model;
	Vec3f position;
	Mat44f model2world;
	Obstacle(Model* model, Vec3f position) {
		this->model = model;
		this->position = position;
		this->model2world = make_translation(position);
	};

	virtual bool isColliding(Vec3f) {
		return false;
	};
};

class SphereObstacle : public Obstacle
{
private:
	float radius;
public:
	SphereObstacle(Model* model, Vec3f position, float radius) : Obstacle(model, position) {
	this->radius = radius;
	this->model2world = make_translation(position) * make_scaling(radius, radius, radius);
	};

	bool isColliding(Vec3f boidPosition) override {
		return length(boidPosition - this->position) < this->radius + 2.f;
	};
};

class BoxObstacle : public Obstacle
{
private:
	Vec3f size; // size of the box from origin
	Vec3f corner1;
	Vec3f corner2;
public:
	BoxObstacle(Model* model, Vec3f position, Vec3f size) : Obstacle(model, position) {
		this->size = size;
		this->model2world = make_translation(position) * make_scaling(size.x, size.y, size.z);
		this->corner1 = this->position - this->size - Vec3f{ 5.f, 5.f, 5.f };
		this->corner2 = this->position + this->size + Vec3f{ 5.f, 5.f, 5.f };
	}

	bool isColliding(Vec3f boidPosition) override {
		return boidPosition.x >= this->corner1.x && boidPosition.x <= this->corner2.x &&
			boidPosition.y >= this->corner1.y && boidPosition.y <= this->corner2.y &&
			boidPosition.z >= this->corner1.z && boidPosition.z <= this->corner2.z;
	}
};
