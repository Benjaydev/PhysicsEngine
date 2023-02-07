#pragma once
#include "PhysicsObject.h"


class Rigidbody : public PhysicsObject {
public:
    Rigidbody(ShapeType _shapeID, glm::vec2 _position, glm::vec2 _velocity, float _orientation, float _mass, float _restitution);
    Rigidbody();
    ~Rigidbody();

    virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
    void Draw() override;
    void ApplyForce(glm::vec2 force, glm::vec2 pos);
    void AddForceToActor(Rigidbody* actor2, glm::vec2 force);
    void ApplyForceToActor(Rigidbody* actor2, glm::vec2 force);

    glm::vec2 GetPosition() { return position; }
    float GetOrientatation() { return orientation; }
    glm::vec2 GetVelocity() { return velocity; }
    float GetMass() { return isKinematic ? INT_MAX : mass; }

    float GetMoment() { return isKinematic ? INT_MAX : moment; }

    void SetVelocity(glm::vec2 _velocity) { velocity = _velocity; }


    void ResolveCollision(Rigidbody* actor2, glm::vec2 contact, glm::vec2* collisionNormal = nullptr, float pen = 0);

    float GetPotentialEnergy();

    glm::vec2 ToWorld(glm::vec2 point);

    glm::vec2 position;
    glm::vec2 velocity;
    float mass;
    float restitution = 1.0f;
    float linearDrag = 0.3f;
    float angularDrag = 0.3f;

    float orientation = 0.f;
    float angularVelocity = 0.f;
    float moment = 1.f;

    // Store the local x,y axes of the box based on its angle of rotation 
    glm::vec2 localX;
    glm::vec2 localY;

    bool isKinematic = false;
    bool eraser = false;

    int frameCount = 0;
};