#pragma once
#include <glm/glm.hpp>
#include "Gizmos.h"

enum ShapeType {
    NONE = -3,
    ERASER = -2,
    JOINT = -1,
    PLANE = 0,
    CIRCLE,
    BOX,
    SHAPE_COUNT

};

class PhysicsObject
{
protected:
    PhysicsObject(ShapeType _shapeID) : shapeID(_shapeID) {}
    PhysicsObject() { shapeID = NONE; }
public:

    virtual void FixedUpdate(glm::vec2 gravity, float timeStep) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    virtual void ResetPosition() {};

    ShapeType GetShapeID() { return shapeID; };

    glm::vec4 colour;

protected:

    ShapeType shapeID = (ShapeType)(-1);
};