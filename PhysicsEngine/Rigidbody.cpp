#pragma once
#include "Rigidbody.h"
#include "PhysicsScene.h"
#include "PhysicsEngine.h"




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
    if (isKinematic) {
        velocity = glm::vec2(0);
        angularVelocity = 0;
        return;
    }
    position += velocity * timeStep;
    ApplyForce(gravity * mass * timeStep, glm::vec2(0, 0));

    orientation += angularVelocity * timeStep;

    velocity -= velocity * linearDrag * timeStep;
    angularVelocity -= angularVelocity * angularDrag * timeStep;

    if (glm::dot(velocity, velocity) < PhysicsEngine::configValues["MIN_LINEAR_THRESHOLD"] * PhysicsEngine::configValues["MIN_LINEAR_THRESHOLD"]) {
        velocity = glm::vec2(0, 0);
    }
    if (abs(angularVelocity) < PhysicsEngine::configValues["MIN_ANGULAR_THRESHOLD"]) {
        angularVelocity = 0;
    }

}

void Rigidbody::ApplyForce(glm::vec2 force, glm::vec2 pos) {
    if (!isKinematic) {
        velocity += force / GetMass();
        angularVelocity += (force.y * pos.x - force.x * pos.y) / GetMoment();
    }
}

void Rigidbody::AddForceToActor(Rigidbody* actor2, glm::vec2 force) {
    /*actor2->ApplyForce(-force * mass);
    ApplyForce(force * mass);*/
}

void Rigidbody::ApplyForceToActor(Rigidbody* actor2, glm::vec2 force)
{
    actor2->velocity = -force / actor2->mass;
    velocity = force / mass;
}

void Rigidbody::ResolveCollision(Rigidbody* actor2, glm::vec2 contact, glm::vec2* collisionNormal, float pen)
{
    //glm::vec2 normal = glm::normalize(actor2->position + actor2->velocity*0.01f - position + velocity * 0.01f);
    glm::vec2 normal = glm::normalize(collisionNormal ? *collisionNormal : actor2->position - position);

    //// One rigidbody is static
    //if (isKinematic || actor2->isKinematic) {
    //    Rigidbody* staticObj = isKinematic ? this : actor2;
    //    Rigidbody* nonStaticObj = isKinematic ? actor2 : this;

    //    float coRestitution = PhysicsEngine::CalculateCoefficientRestitution(staticObj->restitution, nonStaticObj->restitution);
    //    float j = glm::dot(-(1 + coRestitution) * (nonStaticObj->velocity), normal) / (1 / nonStaticObj->GetMass());

    //    glm::vec2 force = normal * j;
    //    nonStaticObj->ApplyForce(force, contact - nonStaticObj->position);
    //}
    glm::vec2 perp(normal.y, -normal.x);

    float r1 = glm::dot(contact - position, -perp);
    float r2 = glm::dot(contact - actor2->position, perp);
    float v1 = glm::dot(velocity, normal) - r1 * angularVelocity;
    float v2 = glm::dot(actor2->velocity, normal) + r2 * actor2->angularVelocity;

    if (v1 > v2) {
        float mass1 = 1.0f / (1.0f / mass + (r1 * r1) / moment);
        float mass2 = 1.0f / (1.0f / actor2->mass + (r2 * r2) / actor2->moment);

        float coRestitution = PhysicsEngine::CalculateCoefficientRestitution(restitution, actor2->restitution);

        glm::vec2 force = (1.0f + 1) * mass1 * mass2 / (mass1 + mass2) * (v1 - v2) * normal;

        ApplyForce(-force, contact - position);
        actor2->ApplyForce(force, contact - actor2->position);
    }

    if (pen > 0) {
        PhysicsScene::ApplyContactForces(this, actor2, normal, pen);
    }


   

}

float Rigidbody::GetPotentialEnergy()
{
    return -mass * glm::dot(PhysicsScene::gravity, position);
}