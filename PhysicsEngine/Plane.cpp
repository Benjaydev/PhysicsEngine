#pragma once
#include "Plane.h"
#include "PhysicsEngine.h"

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


void Plane::Draw() {
    float lineSegmentLength = PhysicsEngine::physicsEngine->orthoSize*2;
    glm::vec2 centerPoint = normal * distanceToOrigin;
    // easy to rotate normal through 90 degrees around z
    glm::vec2 parallel(normal.y, -normal.x);
    glm::vec4 colourFade = colour;
    colourFade.a = 0;
    glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
    glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
    //aie::Gizmos::add2DLine(start, end, colour);
    aie::Gizmos::add2DTri(start, end, start - normal * 10.0f, colour, colour, colourFade);
    aie::Gizmos::add2DTri(end, end - normal * 10.0f, start - normal * 10.0f, colour, colourFade, colourFade);

    if (PhysicsEngine::configSettings["ACTIVE_DEBUG_LINES"] == 1) {
        aie::Gizmos::add2DLine(centerPoint, centerPoint + normal * PhysicsEngine::physicsEngine->orthoSize * 0.02f, glm::vec4(1, 0, 0, 1));
    }
}

void Plane::ResolveCollision(Rigidbody* actor2, glm::vec2 contact)
{
    if (actor2->eraser) {
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

    actor2->ApplyForce(force, contact);

    float pen = glm::dot(contact, normal) - distanceToOrigin;
    PhysicsScene::ApplyContactForces(actor2, nullptr, normal, pen);
}