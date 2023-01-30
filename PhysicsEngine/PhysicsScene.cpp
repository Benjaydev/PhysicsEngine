#include "PhysicsScene.h"

PhysicsScene::PhysicsScene() : fixedDeltaTime(0.01f), m_gravity(glm::vec2(0, 0))
{
}

void PhysicsScene::Update(float dt)
{
    // update physics at a fixed time step

    static float accumulatedTime = 0.0f;
    accumulatedTime += dt;

    while (accumulatedTime >= fixedDeltaTime)
    {
        for (PhysicsObject* pActor : m_actors)
        {
            pActor->FixedUpdate(m_gravity, fixedDeltaTime);
        }

        accumulatedTime -= fixedDeltaTime;
    }
}

void PhysicsScene::Draw()
{
    for (PhysicsObject* pActor : m_actors) {
        pActor->Draw();
    }
}