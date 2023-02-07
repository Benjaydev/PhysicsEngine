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
    localX = glm::vec2(0);
    localY = glm::vec2(0);
}

Rigidbody::Rigidbody()
{
    shapeID = NONE;
    position = glm::vec2(0);
    velocity = glm::vec2(0);;
    orientation = 0;
    mass = 0;
    restitution = 0;
    localX = glm::vec2(0);
    localY = glm::vec2(0);
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::Draw() {
    // Show gravity
    if (PhysicsEngine::configSettings["ACTIVE_DEBUG_LINES"] == 1) {
        aie::Gizmos::add2DLine(position, position + PhysicsScene::gravity, glm::vec4(1, 0.5f, 0, 1));
        aie::Gizmos::add2DLine(position, position + velocity, glm::vec4(1, 1, 0, 1));

        aie::Gizmos::add2DLine(position, position + (GetPotentialEnergy() / 1000), glm::vec4(1, 1, 1, 1));
    }

}


void Rigidbody::FixedUpdate(glm::vec2 gravity, float timeStep)
{
    // Delete self after 2 frames without collision if an eraser
    if (eraser) {
        frameCount++;
        if (frameCount >= 2) {
            PhysicsEngine::physicsEngine->physicsScene->QueueDestroy(this);
        }
    }

    //store the local axes 
    float cs = cosf(orientation);
    float sn = sinf(orientation);
    localX = glm::normalize(glm::vec2(cs, sn));
    localY = glm::normalize(glm::vec2(-sn, cs));

    if (isKinematic) {
        velocity = glm::vec2(0);
        angularVelocity = 0;
        return;
    }

    position += velocity * timeStep;
    ApplyForce(gravity * mass * timeStep, position);

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

glm::vec2 Rigidbody::ToWorld(glm::vec2 point) {
    return position + (localX * point.x + localY * point.y);
}

void Rigidbody::ApplyForce(glm::vec2 force, glm::vec2 pos) {
    if (!isKinematic) {
        velocity += force / GetMass();
        glm::vec2 offset = pos - position;
        angularVelocity += (force.y * offset.x - force.x * offset.y) / GetMoment();
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
    // Object should erase other object it collides with
    if (eraser) {
        frameCount = 0;
        PhysicsEngine::physicsEngine->physicsScene->QueueDestroy(actor2);
        return;
    }
    else if (actor2->eraser) {
        actor2->frameCount = 0;
        PhysicsEngine::physicsEngine->physicsScene->QueueDestroy(this);
        return;
    }

    glm::vec2 normal = glm::normalize(collisionNormal ? *collisionNormal : actor2->position - position);

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

        ApplyForce(-force, contact);
        actor2->ApplyForce(force, contact);
    }

    if (pen > 0) {
        PhysicsScene::ApplyContactForces(this, actor2, normal, pen);
    }


   

}

float Rigidbody::GetPotentialEnergy()
{
    return -mass * glm::dot(PhysicsScene::gravity, position);
}