#include "Rigidbody.h"




Rigidbody::Rigidbody(ShapeType _shapeID, glm::vec2 _position, glm::vec2 _velocity, float _orientation, float _mass)
{
    shapeID = _shapeID;
    position = _position;
    velocity = _velocity;
    orientation = _orientation;
    mass = _mass;
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

}

void Rigidbody::ApplyForceToActor(Rigidbody* actor2, glm::vec2 force) {

}