#pragma once
#include "Plane.h"
#include "PhysicsEngine.h"
#include <iostream>

Plane::Plane() : 
    PhysicsObject(PLANE) {
    distanceToOrigin = 0;
    normal = glm::vec2(0, 1);
    colour = glm::vec4(1, 1, 1, 1);
}

Plane::Plane(glm::vec2 _normal, float _distance, float _restitution, glm::vec4 _colour) :
    PhysicsObject(PLANE) {
    distanceToOrigin = _distance;
    normal = glm::normalize(_normal);
    colour = _colour;
    restitution = _restitution;
    
}

void Plane::Update(float deltaTime) {
}

void Plane::Draw() {
    float doubleOrthoSize = (PhysicsEngine::physicsEngine->orthoSize * 2) + abs(PhysicsEngine::physicsEngine->orthoCenter.x) + abs(PhysicsEngine::physicsEngine->orthoCenter.y);
    float lineSegmentLength = doubleOrthoSize;
    glm::vec2 centerPoint = normal * distanceToOrigin;
    // easy to rotate normal through 90 degrees around z
    glm::vec2 parallel(normal.y, -normal.x);
    glm::vec4 colourFade = colour;
    colourFade.a = 0.f;
    glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
    glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
    //aie::Gizmos::add2DLine(start, end, colour);
    aie::Gizmos::add2DTri(start, end, start - normal * doubleOrthoSize, colour, colour, colourFade);
    aie::Gizmos::add2DTri(end, end - normal * doubleOrthoSize, start - normal * doubleOrthoSize, colour, colourFade, colourFade);

    if (PhysicsEngine::configSettings["ACTIVE_DEBUG_LINES"] == 1) {
        aie::Gizmos::add2DLine(centerPoint, centerPoint + normal * PhysicsEngine::physicsEngine->orthoSize * 0.02f, glm::vec4(1, 0, 0, 1));
    }
}

void Plane::ResolveCollision(Rigidbody* actor2, glm::vec2 contact)
{
    if (actor2->isEraser) {
        actor2->frameCount = 0;
        PhysicsEngine::physicsEngine->physicsScene->QueueDestroy(this);
        return;
    }


    glm::vec2 localContact = contact - actor2->position;
    glm::vec2 vRel = actor2->velocity + actor2->angularVelocity * glm::vec2(-localContact.y, localContact.x);
    

    float velocityIntoPlane = glm::dot(vRel, normal);

    float coRestitution = PhysicsEngine::CalculateCoefficientRestitution(restitution, actor2->restitution);

    float r = glm::dot(localContact, glm::vec2(normal.y, -normal.x));

    float mass0 = 1.0f / (1.0f / actor2->GetMass() + (r * r) / actor2->GetMoment());

    float j = -(1 + coRestitution) * velocityIntoPlane * mass0;

    glm::vec2 force = normal * j;

    if (!actor2->isTrigger) {
        actor2->ApplyForce(force, contact);

        if (actor2->collisionCallback)
            actor2->collisionCallback(this);
    }
    else {
        actor2->triggerEnterCallback(this);
    }


    // Friction impulse
    vRel = actor2->velocity + actor2->angularVelocity * glm::vec2(-localContact.y, localContact.x);
    glm::vec2 tangent = vRel - glm::dot(vRel, normal) * normal;

    // If friction is above minimum threshold
    if (glm::dot(tangent, tangent) > PhysicsEngine::configValues["V_MIN_FRICTION_THRESHOLD"] * PhysicsEngine::configValues["V_MIN_FRICTION_THRESHOLD"]) {
        tangent =  glm::normalize(tangent);
        float jt = -glm::dot(vRel, tangent) / mass0;

        float mu = PhysicsEngine::CalculateCoefficientStaticFriction(staticFriction, actor2->staticFriction);
        glm::vec2 frictionImpulse;
        // If energy is low enough, assume object is at or nearly at rest 
        if (abs(jt) < j * mu) {
            frictionImpulse = jt * tangent;
        }
        // If higher, the object has already broken the initial energy of activation
        else {
            float dynamFriction = PhysicsEngine::CalculateCoefficientStaticFriction(staticFriction, actor2->dynamicFriction);
            frictionImpulse = -j * tangent * dynamFriction;
        }

        // Apply the friction
        if (!actor2->isTrigger) {
            actor2->ApplyForce((mass0 * frictionImpulse), contact);
        }
    }


    float pen = glm::dot(contact, normal) - distanceToOrigin;
    PhysicsScene::ApplyContactForces(actor2, nullptr, normal, pen);
}