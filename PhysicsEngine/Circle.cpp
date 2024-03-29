#pragma once
#include "Circle.h"
#include "Application.h"
#include "PhysicsEngine.h"

Circle::Circle(glm::vec2 _position, glm::vec2 _velocity, float _mass, float _radius, float _restitution, glm::vec4 _colour) :
    Rigidbody(CIRCLE, _position, _velocity, 0, _mass, _restitution)
{
    radius = _radius;
    colour = _colour;
}

Circle::~Circle()
{
}

void Circle::FixedUpdate(glm::vec2 gravity, float timeStep) {
    moment = (0.5f) * mass * (radius * radius);
    Rigidbody::FixedUpdate(gravity, timeStep);

}

void Circle::Update(float deltaTime) {
    Rigidbody::Update(deltaTime);
}

void Circle::Draw()
{
    aie::Gizmos::add2DCircle(visualPosition, radius, 12, colour);

    if (PhysicsEngine::configSettings["ACTIVE_DEBUG_LINES"] == 1) {
        glm::vec2 end = glm::vec2(std::cos(visualOrientation), std::sin(visualOrientation)) *
            radius;
        aie::Gizmos::add2DLine(visualPosition, visualPosition + end, glm::vec4(1, 1, 1, 1));
    }
    Rigidbody::Draw();

}

void Circle::CalculateBounds()
{
    minBounds.x = position.x - radius;
    minBounds.y = position.y - radius;

    maxBounds.x = position.x + radius;
    maxBounds.y = position.y + radius;
}
