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

	virtual bool isColliding(Vec3f position) {
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

	bool isColliding(Vec3f position) override {
		return length(position - this->position) < this->radius + 2.f;
	};
};

//class BoxObstacle : public Obstacle
//{
//private:
//	Vec3f size; // size of the box from origin
//	Vec3f corner1;
//	Vec3f corner2;
//public:
//	BoxObstacle(Model* model, Vec3f position, Vec3f size) : Obstacle(model, position) {
//		this->size = size;
//		this->model2world = make_translation(position) * make_scaling(size.x, size.y, size.z);
//		this->corner1 = this->position - this->size / 2.f - Vec3f{ 2.f, 2.f, 2.f };
//		this->corner2 = this->position + this->size / 2.f + Vec3f{ 2.f, 2.f, 2.f };
//	}
//
//	bool isColliding(Vec3f position) override {
//		return position.x >= this->corner1.x && position.x <= this->corner2.x &&
//			position.y >= this->corner1.y && position.y <= this->corner2.y &&
//			position.z >= this->corner1.z && position.z <= this->corner2.z;
//	}
//};
