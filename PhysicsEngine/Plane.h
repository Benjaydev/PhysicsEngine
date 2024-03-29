#pragma once
#include "Rigidbody.h"

class Plane : public PhysicsObject
{
public:
    Plane();
    Plane(glm::vec2 _normal, float _distance, float _restitution, glm::vec4 _colour);
    ~Plane();

    void FixedUpdate(glm::vec2 gravity, float timeStep) override {};
    void Update(float deltaTime) override;
    void Draw() override;
    void ResetPosition() override {};

    glm::vec2 GetNormal() { return normal; }
    float GetDistance() { return distanceToOrigin; }

    void ResolveCollision(Rigidbody* actor2, glm::vec2 contact);

    glm::vec2 normal;
    float distanceToOrigin;
    float restitution = 1.0f;
    float staticFriction = 0.7f;

protected:

};