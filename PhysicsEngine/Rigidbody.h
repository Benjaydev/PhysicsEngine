#pragma once
#include "PhysicsObject.h"

class Rigidbody : public PhysicsObject {
public:
    Rigidbody(ShapeType _shapeID, glm::vec2 _position, glm::vec2 _velocity, float _orientation, float _mass);
    ~Rigidbody();

    virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
    void ApplyForce(glm::vec2 force);
    void ApplyForceToActor(Rigidbody* actor2, glm::vec2 force);

    glm::vec2 GetPosition() { return position; }
    float GetOrientatation() { return orientation; }
    glm::vec2 GetVelocity() { return velocity; }
    float GetMass() { return mass; }

protected:
    glm::vec2 position;
    glm::vec2 velocity;
    float mass;
    float orientation;    //2D so we only need a single float to represent our orientation
};