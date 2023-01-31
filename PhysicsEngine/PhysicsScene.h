#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Circle.h"
#include "Plane.h"


class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void AddActor(PhysicsObject* actor);
	void RemoveActor(PhysicsObject* actor);
	void Update(float dt);
	void Draw();
	void CheckCollisions();

	void SetGravity(const glm::vec2 _gravity) { gravity = _gravity; }
	glm::vec2 GetGravity() const { return gravity; }

	void SetFixedDeltaTime(const float timeStep) { fixedDeltaTime = timeStep; }
	float GetFixedDeltaTime() const { return fixedDeltaTime; }

	static bool Plane2Plane(PhysicsObject* obj1, PhysicsObject* obj2) { return false; };
	static bool Plane2Circle(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Circle2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Circle2Circle(PhysicsObject* obj1, PhysicsObject* obj2);

protected:
	glm::vec2 gravity;
	float fixedDeltaTime;
	float deltaTime = 0.0f;
	std::vector<PhysicsObject*> sceneActors;
};
