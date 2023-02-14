#pragma once
#include "Rigidbody.h"
#include "PhysicsScene.h"
#include "PhysicsEngine.h"

Rigidbody::Rigidbody(ShapeType _shapeID, glm::vec2 _position, glm::vec2 _velocity, float _orientation, float _mass, float _restitution) : PhysicsObject(_shapeID)
{
    position = _position;
    velocity = _velocity;
    orientation = _orientation;
    mass = _mass;
    restitution = _restitution;
    float cs = cosf(orientation);
    float sn = sinf(orientation);
    localX = glm::normalize(glm::vec2(cs, sn));
    localY = glm::normalize(glm::vec2(-sn, cs));

    // By default, call onCollider function when a collision occurs
    collisionCallback = std::bind(&Rigidbody::OnCollide, this, std::placeholders::_1);

    triggerEnterCallback = std::bind(&Rigidbody::OnTriggerEnter, this, std::placeholders::_1);
    triggerExitCallback = std::bind(&Rigidbody::OnTriggerExit, this, std::placeholders::_1);
}

Rigidbody::Rigidbody() : PhysicsObject(NONE)
{
    position = glm::vec2(0);
    velocity = glm::vec2(0);;
    orientation = 0;
    mass = 0;
    restitution = 0;
    float cs = cosf(orientation);
    float sn = sinf(orientation);
    localX = glm::normalize(glm::vec2(cs, sn));
    localY = glm::normalize(glm::vec2(-sn, cs));

    // By default, call onCollider function when a collision occurs
    collisionCallback = std::bind(&Rigidbody::OnCollide, this, std::placeholders::_1);

    triggerEnterCallback = std::bind(&Rigidbody::OnTriggerEnter, this, std::placeholders::_1);
    triggerExitCallback = std::bind(&Rigidbody::OnTriggerExit, this, std::placeholders::_1);
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::Draw() {
    // Show gravity
    if (PhysicsEngine::configSettings["ACTIVE_DEBUG_LINES"] == 1 && !eraser) {
        aie::Gizmos::add2DLine(visualPosition, visualPosition + PhysicsScene::gravity, glm::vec4(1, 0.5f, 0, 1));
        aie::Gizmos::add2DLine(visualPosition, visualPosition + velocity, glm::vec4(1, 1, 0, 1));

        aie::Gizmos::add2DLine(visualPosition, visualPosition + (GetPotentialEnergy() / 1000), glm::vec4(1, 1, 1, 1));
    }

}


void Rigidbody::Update(float deltaTime) {
    if (PhysicsEngine::configSettings["SHOULD_LERP_POSITIONS"] == 1) {
        deltaTimeCount += deltaTime;
        float visualLerpAlpha = (deltaTimeCount / fixedTimeStore);
        // Update the visual position outside seperately from physics updates
        visualPosition = lastPosition + ((position - lastPosition) * visualLerpAlpha);
        visualOrientation = lastOrientation + ((orientation - lastOrientation) * visualLerpAlpha);

        visualLocalX = lastLocalX;
        visualLocalY = lastLocalY;

    }
 
}

void Rigidbody::FixedUpdate(glm::vec2 gravity, float timeStep)
{
    // Delete self after 2 frames without collision if an eraser
    if (eraser) {
        frameCount++;
        if (frameCount >= 4) {
            PhysicsEngine::physicsEngine->physicsScene->QueueDestroy(this);
        }
    }

    lastLocalX = localX;
    lastLocalY = localY;

    // Store the local axes 
    float cs = cosf(orientation);
    float sn = sinf(orientation);
    localX = glm::normalize(glm::vec2(cs, sn));
    localY = glm::normalize(glm::vec2(-sn, cs));


    // Trigger checks 
    if (isTrigger)
    {
        // Check every object that is inside us and called triggerEnter on 
        // if they haven’t registered inside us this frame, they must have exited 
        // so remove them from our list and call triggerExit 
        for (auto it = objectsInside.begin(); it != objectsInside.end(); it++)
        {
            if (std::find(objectsInsideThisFrame.begin(),
                objectsInsideThisFrame.end(), *it) == objectsInsideThisFrame.end())
            {
                if (triggerExitCallback)
                    triggerExitCallback(*it);
                it = objectsInside.erase(it);
                if (it == objectsInside.end())
                    break;
            }
        }
    }

    // clear this list now for next frame 
    objectsInsideThisFrame.clear();

    if (isKinematic) {
        velocity = glm::vec2(0);
        angularVelocity = 0;
    }

    // keep track of the physics update step
    fixedTimeStore = timeStep;
    // Reset deltaTime count
    deltaTimeCount = 0;


    lastPosition = position;
    position += velocity * timeStep;
    ApplyForce(gravity * mass * timeStep, position);

    lastOrientation = orientation;
    orientation += angularVelocity * timeStep;

    velocity -= velocity * linearDrag * timeStep;
    angularVelocity -= angularVelocity * angularDrag * timeStep;

    if (glm::dot(velocity, velocity) < PhysicsEngine::configValues["MIN_LINEAR_THRESHOLD"] * PhysicsEngine::configValues["MIN_LINEAR_THRESHOLD"]) {
        velocity = glm::vec2(0, 0);
    }
    if (abs(angularVelocity) < PhysicsEngine::configValues["MIN_ANGULAR_THRESHOLD"]) {
        angularVelocity = 0;
    }

    // Positions shouldn't lerp
    if (PhysicsEngine::configSettings["SHOULD_LERP_POSITIONS"] != 1) {
        visualPosition = position;
        visualOrientation = orientation;
        visualLocalX = localX;
        visualLocalY = localY;
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
    // register that these two objects have overlapped this frame 
    objectsInsideThisFrame.push_back(actor2);
    actor2->objectsInsideThisFrame.push_back(this);

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

        float j = (1.0f + coRestitution) * mass1 * mass2 / (mass1 + mass2) * (v1 - v2);
        glm::vec2 force = normal * j;

        if (!isTrigger && !actor2->isTrigger) {
            ApplyForce(-force, contact);
            actor2->ApplyForce(force, contact);

            if (collisionCallback != nullptr)
                collisionCallback(actor2);
            if (actor2->collisionCallback)
                actor2->collisionCallback(this);
        }
        else
        {
            TriggerEnter(actor2);
            actor2->TriggerEnter(this);
        }


        // Friction impulse
        glm::vec2 relV = (actor2->velocity + actor2->angularVelocity - velocity + angularVelocity);
        glm::vec2 tangent = relV - glm::dot(relV, normal) * normal;
        if (glm::dot(tangent, tangent) > PhysicsEngine::configValues["V_MIN_FRICTION_THRESHOLD"] * PhysicsEngine::configValues["V_MIN_FRICTION_THRESHOLD"]) {
            tangent = glm::normalize(tangent);

            float jt = -glm::dot(relV, tangent) / (1 / mass1 + 1 / mass2);

            float mu = PhysicsEngine::CalculateCoefficientStaticFriction(staticFriction, actor2->staticFriction);

            glm::vec2 frictionImpulse;
            // If energy is low enough, assume object is at or nearly at rest 
            if (abs(jt) < j * mu) {
                frictionImpulse = jt * tangent;
            }
            // If higher, the object has already broken the initial energy of activation
            else {
                float dynamFriction = PhysicsEngine::CalculateCoefficientStaticFriction(dynamicFriction, actor2->dynamicFriction);
                frictionImpulse = -j * tangent * dynamFriction;
            }

            // Apply the friction
            if (!isTrigger && !actor2->isTrigger) {
                ApplyForce(-((1 / mass) * frictionImpulse), contact);
                actor2->ApplyForce(((1 / actor2->mass) * frictionImpulse), contact);
            }
        }
    }

    if (pen > 0) {
        PhysicsScene::ApplyContactForces(this, actor2, normal, pen);
    }

}

void Rigidbody::TriggerEnter(PhysicsObject* actor2)
{
    if (isTrigger && std::find(objectsInside.begin(), objectsInside.end(), actor2) == objectsInside.end())
    {
        objectsInside.push_back(actor2);
        if (triggerEnterCallback != nullptr)
            triggerEnterCallback(actor2);
    }
}

float Rigidbody::GetPotentialEnergy()
{
    return -mass * glm::dot(PhysicsScene::gravity, position);
}