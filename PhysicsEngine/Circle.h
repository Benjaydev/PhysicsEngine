#include "Rigidbody.h"

class Circle : public Rigidbody
{
public:
    Circle(glm::vec2 _position, glm::vec2 _velocity,
        float _mass, float _radius, glm::vec4 _colour);
    ~Circle();

    void Draw() override;

    float GetRadius() { return radius; }
    glm::vec4 GetColour() { return colour; }

protected:
    float radius;
    glm::vec4 colour;
};