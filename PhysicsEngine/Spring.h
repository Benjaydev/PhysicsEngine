#pragma once
#include "Rigidbody.h"

class Spring : public PhysicsObject
{
public:
    Spring(Rigidbody* _body1, Rigidbody* _body2,
        float _springCoefficient, float _damping, float _restLength = 0,
        glm::vec2 _contact1 = glm::vec2(0, 0), glm::vec2 _contact2 = glm::vec2(0, 0));
    ~Spring();

    void FixedUpdate(glm::vec2 gravity, float timeStep) override;
    void Draw() override;

    glm::vec4 GetColour() { return colour; }

    glm::vec2 GetContact1();
    glm::vec2 GetContact2();


    Rigidbody* body1;
    Rigidbody* body2;

    glm::vec2 contact1;
    glm::vec2 contact2;

    float damping;
    float restLength;
    float springCoefficient;  // the restoring force

protected:
    glm::vec4 colour = glm::vec4(1,1,1,1);
};