#pragma once
#include <glm/glm.hpp>
#include "Gizmos.h"

enum ShapeType {
    NONE = -1,
    PLANE = 0,
    CIRCLE,
    //BOX,
    SHAPE_COUNT

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

    ShapeType GetShapeID() { return shapeID; };

protected:
    ShapeType shapeID = (ShapeType)(-1);
};