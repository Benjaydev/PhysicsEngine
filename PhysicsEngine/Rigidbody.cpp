#include "Rigidbody.h"




Rigidbody::Rigidbody(ShapeType _shapeID, glm::vec2 _position, glm::vec2 _velocity, float _orientation, float _mass, float _restitution)
{
    shapeID = _shapeID;
    position = _position;
    velocity = _velocity;
    orientation = _orientation;
    mass = _mass;
    restitution = _restitution;
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::FixedUpdate(glm::vec2 gravity, float timeStep)
{
    position += velocity * timeStep;
    ApplyForce(gravity * mass * timeStep);
}

void Rigidbody::ApplyForce(glm::vec2 force) {
    velocity += force / mass;
}

void Rigidbody::ApplyForceToActor(Rigidbody* actor2, glm::vec2 force) {
    actor2->ApplyForce(-force * mass);
    ApplyForce(force * mass);
}