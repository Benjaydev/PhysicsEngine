#pragma once
#include "PhysicsScene.h"

PhysicsScene::PhysicsScene() : gravity(glm::vec2(0, -10))
{
    fixedDeltaTime = 0.01f;
}

PhysicsScene::~PhysicsScene()
{
    for (PhysicsObject* pActor : sceneActors)
    {
        delete pActor;
    }
}

void PhysicsScene::AddActor(PhysicsObject* actor)
{
    sceneActors.push_back(actor);
}

void PhysicsScene::RemoveActor(PhysicsObject* actor)
{
    auto iter = std::find(sceneActors.begin(), sceneActors.end(), actor);
    if (iter != sceneActors.end()) {
        sceneActors.erase(iter);
    }

}
void PhysicsScene::Draw()
{
    for (PhysicsObject* pActor : sceneActors) {
        pActor->Draw();
    }
}


// function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);


/*
column = 1st object
Row = 2nd object
            Plane Circle
    Plane
    Circle


*/
static fn collisionFunctionArray[] =
{
    PhysicsScene::Plane2Plane,  PhysicsScene::Plane2Circle,
    PhysicsScene::Circle2Plane, PhysicsScene::Circle2Circle,
};


void PhysicsScene::Update(float dt)
{
    // update physics at a fixed time step
    deltaTime = dt;
    static float accumulatedTime = 0.0f;
    accumulatedTime += deltaTime;

    while (accumulatedTime >= fixedDeltaTime)
    {
        for (PhysicsObject* pActor : sceneActors)
        {
            pActor->FixedUpdate(gravity, fixedDeltaTime);
        }

        accumulatedTime -= fixedDeltaTime;


        CheckCollisions();
    }
}


void PhysicsScene::CheckCollisions()
{
    int actorCount = sceneActors.size();

    // need to check for collisions against all objects except this one.
    for (int outer = 0; outer < actorCount - 1; outer++)
    {
        for (int inner = outer + 1; inner < actorCount; inner++)
        {
            PhysicsObject* object1 = sceneActors[outer];
            PhysicsObject* object2 = sceneActors[inner];

            int shapeId1 = object1->GetShapeID();
            int shapeId2 = object2->GetShapeID();

            // using function pointers
            int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
            fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
            if (collisionFunctionPtr != nullptr)
            {
                // did a collision occur?
                collisionFunctionPtr(object1, object2);
            }
        }
    }
}

bool PhysicsScene::Circle2Circle(PhysicsObject* obj1, PhysicsObject* obj2)
{
    // try to cast objects to Circle and Circle
    Circle* circle1 = dynamic_cast<Circle*>(obj1);
    Circle* circle2 = dynamic_cast<Circle*>(obj2);
    // if we are successful then test for collision
    if (circle1 != nullptr && circle2 != nullptr)
    {
        glm::vec2 diff = circle2->position - circle1->position;
        float sqrMag = glm::dot(diff, diff);
        float combRadi = (circle1->radius + circle2->radius);

        glm::vec2 combinedVel = circle1->velocity + circle2->velocity;
        glm::vec2 circle1Ratio = circle1->velocity / combinedVel;
        glm::vec2 circle2Ratio = circle2->velocity / combinedVel;

        float intersectionDepth = sqrMag - (combRadi * combRadi);
        if (intersectionDepth < 0) {
            // REPLACE 0.01 with staticDeltaTime!
            circle1->position -= circle1->velocity * circle1Ratio * 0.01f;
            circle2->position -= circle2->velocity * circle2Ratio * 0.01f;


            glm::vec2 normal = diff / sqrt(sqrMag);

            float j = (-(1 + (0.9f)) * glm::dot(circle2->velocity - circle1->velocity, normal)) /
                    (((1 / circle1->mass) + (1 / circle2->mass)) * glm::dot(normal, normal));

            circle1->SetVelocity(circle1->velocity - j * normal / circle1->mass);
            circle2->SetVelocity(circle2->velocity + j * normal / circle2->mass);
        }

    }
    return false;
}

bool PhysicsScene::Circle2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
    Circle* circle = dynamic_cast<Circle*>(obj1);
    Plane* plane = dynamic_cast<Plane*>(obj2);
    //if we are successful then test for collision
    if (circle != nullptr && plane != nullptr)
    {
        glm::vec2 collisionNormal = plane->normal;
        float sphereToPlane = glm::dot(circle->position, plane->normal) - plane->distanceToOrigin - circle->radius;

        float velocityOutOfPlane = glm::dot(circle->velocity, plane->normal);
        if (sphereToPlane < 0 && velocityOutOfPlane < 0)
        {

            circle->SetVelocity(circle->velocity - (1+circle->restitution) * glm::dot(circle->velocity, plane->normal) * plane->normal);
            //set sphere velocity to zero here
            //circle->ApplyForce(-circle->velocity * circle->mass);




            return true;
        }
    }
    return false;
}

bool PhysicsScene::Plane2Circle(PhysicsObject* obj1, PhysicsObject* obj2) {
    return Circle2Plane(obj2, obj1);
}
