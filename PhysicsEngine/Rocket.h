#include <glm/glm.hpp>
#include "PhysicsScene.h"

class Rocket
{
public:
    Rocket(float _fireRate, float _fireAmount, glm::vec2 _force) : fireRate(_fireRate), fireAmount(_fireAmount), force(_force) 
    {
       ship = new Circle(glm::vec2(0, 0), glm::vec2(0, 0), 3.0f, 10.0f, 1.0f, glm::vec4(1, 0, 0, 1));
    };
    ~Rocket();

    void Update(PhysicsScene* scene, float deltaTime);

    Circle* ship;

protected:

    float count = 0.0f;
    float fireRate;
    float fireAmount;
    glm::vec2 force;
};