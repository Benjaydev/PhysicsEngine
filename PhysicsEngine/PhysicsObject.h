#pragma once
#include <glm/glm.hpp>

enum ShapeType {
    NONE = -1,
    PLANE = 0,
    CIRCLE,
    BOX
};

class PhysicsObject
{
protected:
    PhysicsObject(ShapeType _shapeID) : shapeID(_shapeID) {}
    PhysicsObject() {};
public:

    virtual void FixedUpdate(glm::vec2 gravity, float timeStep) = 0;
    virtual void Draw() = 0;
    virtual void ResetPosition() {};

protected:
    ShapeType shapeID = (ShapeType)(-1);
};