#include "Circle.h"


Circle::Circle(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) :
    Rigidbody(CIRCLE, position, velocity, 0, mass)
{
    radius = radius;
    colour = colour;
}

Circle::~Circle()
{
}

void Circle::Draw()
{
}
