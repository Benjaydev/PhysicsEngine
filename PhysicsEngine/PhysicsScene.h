#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Circle.h"
#include "Box.h"
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

	void SetFixedDeltaTime(const float timeStep) { fixedDeltaTime = timeStep; }
	float GetFixedDeltaTime() const { return fixedDeltaTime; }

	static glm::vec2 NearestPointOnLine(glm::vec2 linePoint, glm::vec2 dir, glm::vec2 checkPoint);
	static void ApplyContactForces(Rigidbody* body1, Rigidbody* body2, glm::vec2 norm, float pen);
	static bool Plane2Plane(PhysicsObject* obj1, PhysicsObject* obj2) { return false; };
	static bool Plane2Circle(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Circle2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Circle2Circle(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Circle2Box(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Circle(PhysicsObject* obj1, PhysicsObject* obj2);
	static bool Box2Box(PhysicsObject* obj1, PhysicsObject* obj2);

	static float fixedDeltaTime;
	float deltaTime = 0.0f;
	static glm::vec2 gravity;


protected:

	std::vector<PhysicsObject*> sceneActors;
};

