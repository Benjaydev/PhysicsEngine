#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "PhysicsObject.h"


class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void AddActor(PhysicsObject* actor);
	void RemoveActor(PhysicsObject* actor);
	void Update(float dt);
	void Draw();

	void SetGravity(const glm::vec2 _gravity) { gravity = _gravity; }
	glm::vec2 GetGravity() const { return gravity; }

	void SetFixedDeltaTime(const float timeStep) { fixedDeltaTime = timeStep; }
	float GetFixedDeltaTime() const { return fixedDeltaTime; }

protected:
	glm::vec2 gravity;
	float fixedDeltaTime;
	std::vector<PhysicsObject*> m_actors;
};
