#include "Circle.h"
#include "Application.h"

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

void Circle::Draw()
{
    glm::vec2 end = glm::vec2(std::cos(orientation), std::sin(orientation)) *
        radius;

    aie::Gizmos::add2DCircle(position, radius, 12, colour);
    aie::Gizmos::add2DLine(position, position + end, glm::vec4(1, 1, 1, 1));
}
