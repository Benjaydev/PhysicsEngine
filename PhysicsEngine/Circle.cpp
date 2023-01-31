#include "Circle.h"

Circle::Circle(glm::vec2 _position, glm::vec2 _velocity, float _mass, float _radius, float _restitution, glm::vec4 _colour) :
    Rigidbody(CIRCLE, _position, _velocity, 0, _mass, _restitution)
{
    radius = _radius;
    colour = _colour;
}

Circle::~Circle()
{
}

void Circle::Draw()
{
    aie::Gizmos::add2DCircle(position, radius, 10, colour);
}
