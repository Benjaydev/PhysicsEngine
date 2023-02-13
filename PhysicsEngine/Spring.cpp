#pragma once
#include "Spring.h"

Spring::Spring(Rigidbody* _body1, Rigidbody* _body2, float _springCoefficient, float _damping, float _restLength, glm::vec2 _contact1, glm::vec2 _contact2) :
	PhysicsObject(JOINT) {
	body1 = _body1;
	body2 = _body2;
	springCoefficient = _springCoefficient;
	damping = _damping;

	if (restLength == 0) {
		glm::vec2 diff = (GetContact2() - GetContact1());
		restLength = glm::length(diff);
	}

	restLength = _restLength;
	contact1 = _contact1;
	contact2 = _contact2;
}


void Spring::FixedUpdate(glm::vec2 gravity, float timeStep) {
	// get the world coordinates of the ends of the springs 
	glm::vec2 p1 = GetContact1();
	glm::vec2 p2 = GetContact2();

	float length = glm::distance(p1, p2);
	glm::vec2 direction = glm::normalize(p2 - p1);

	// apply damping 
	glm::vec2 relativeVelocity = body2->velocity - body1->velocity;

	// F = -kX - bv 
	glm::vec2 force = direction * springCoefficient * (restLength - length) -
		damping * relativeVelocity;

	// cap the spring force to 1000 N to prevent numerical instability 
	const float threshold = 1000.0f;
	float forceMag = glm::length(force);
	if (forceMag > threshold)
		force *= threshold / forceMag;

	body1->ApplyForce(-force * timeStep, p1);
	body2->ApplyForce(force * timeStep, p2);
}
void Spring::Update(float deltaTime) {
}

glm::vec2 Spring::GetContact1()
{
	return body1 ? body1->ToWorld(contact1) : contact1;
}

glm::vec2 Spring::GetContact2()
{
	return body2 ? body2->ToWorld(contact2) : contact2;
}

void Spring::Draw() {
	aie::Gizmos::add2DLine(GetContact1(), GetContact2(), colour);
}