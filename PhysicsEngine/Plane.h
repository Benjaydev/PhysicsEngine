#include "PhysicsObject.h"

class Plane : public PhysicsObject
{
public:
    Plane();
    Plane(glm::vec2 _normal, float _distance, glm::vec4 _colour);
    ~Plane();

    void FixedUpdate(glm::vec2 gravity, float timeStep) override {};
    void Draw() override;
    void ResetPosition() override {};

    glm::vec2 GetNormal() { return normal; }
    float GetDistance() { return distanceToOrigin; }

    glm::vec2 normal;
    float distanceToOrigin;
    glm::vec4 colour;

protected:

};