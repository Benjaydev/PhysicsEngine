#include "Rigidbody.h"

class Circle : public Rigidbody
{
public:
    Circle(glm::vec2 _position, glm::vec2 _velocity,
        float _mass, float _radius, float _restitution, glm::vec4 _colour);
    ~Circle();

    void Draw() override;

    float GetRadius() { return radius; }
    glm::vec4 GetColour() { return colour; }

    float radius;

protected:

    glm::vec4 colour;
};