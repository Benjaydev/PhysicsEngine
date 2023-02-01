#include "Rigidbody.h"
#include "PhysicsScene.h"




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
    if (!isStatic) {
        position += velocity * timeStep;
        ApplyForce(gravity * mass * timeStep);
    }

}

void Rigidbody::ApplyForce(glm::vec2 force) {
    if (!isStatic) {
        velocity += force / mass;
    }
}

void Rigidbody::AddForceToActor(Rigidbody* actor2, glm::vec2 force) {
    actor2->ApplyForce(-force * mass);
    ApplyForce(force * mass);
}

void Rigidbody::ApplyForceToActor(Rigidbody* actor2, glm::vec2 force)
{
    actor2->velocity = -force / actor2->mass;
    velocity = force / mass;
}

void Rigidbody::ResolveCollision(Rigidbody* actor2)
{
    glm::vec2 normal = glm::normalize(actor2->position + actor2->velocity*0.01f - position + velocity * 0.01f);
    glm::vec2 relativeVelocity = actor2->velocity - velocity;

    float coRestitution = (restitution * actor2->restitution);
    float j = (-(1 + (coRestitution)) * glm::dot(relativeVelocity, normal)) /
                    (((1 / mass) + (1 / actor2->mass)));

    glm::vec2 force = normal * j;

    AddForceToActor(actor2, -j * normal / mass);


}

float Rigidbody::GetPotentialEnergy()
{
    return -mass * glm::dot(PhysicsScene::gravity, position);
}