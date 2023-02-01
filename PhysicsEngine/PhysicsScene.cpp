#pragma once
#include "PhysicsScene.h"

float PhysicsScene::fixedDeltaTime = 0.01f;
glm::vec2 PhysicsScene::gravity = glm::vec2(0, -10);

PhysicsScene::PhysicsScene() 
{
    fixedDeltaTime = 0.01f;
    gravity = glm::vec2(0, -10);
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

glm::vec2 PhysicsScene::NearestPointOnLine(glm::vec2 linePoint, glm::vec2 dir, glm::vec2 checkPoint)
{
    dir = glm::normalize(dir);
    glm::vec2 v = checkPoint - linePoint;
    float d = glm::dot(v, dir);
    return linePoint + dir * d;
}

bool PhysicsScene::Circle2Circle(PhysicsObject* obj1, PhysicsObject* obj2)
{
    // try to cast objects to Circle and Circle
    Circle* circle1 = dynamic_cast<Circle*>(obj1);
    Circle* circle2 = dynamic_cast<Circle*>(obj2);
    // if we are successful then test for collision
    if (circle1 != nullptr && circle2 != nullptr)
    {
        // Test for other collision
        glm::vec2 diff = (circle2->position + circle2->velocity * fixedDeltaTime) - (circle1->position + circle1->velocity * 0.01f);
        float sqrMag = glm::dot(diff, diff);
        float combRadi = (circle1->radius + circle2->radius);

        float intersectionDepth = sqrMag - (combRadi * combRadi);
        float dir = glm::dot((circle1->velocity - circle2->velocity), (circle1->position - circle2->position));
        if (intersectionDepth < 0 && dir < 0) {
            float coRestitution = (circle1->restitution * circle2->restitution);
            glm::vec2 staticVec = glm::vec2(0, 0);
            glm::vec2 normal = diff / sqrt(sqrMag);


            // Static-static
            if (circle1->velocity == staticVec && circle2->velocity == staticVec) {
                glm::vec2 midPoint = glm::vec2((circle1->position.x + circle2->position.x) / 2, (circle1->position.y + circle2->position.y) / 2);
                circle1->position.x = midPoint.x + circle1->radius * (circle1->position.x - circle2->position.x) / intersectionDepth;
                circle1->position.y = midPoint.y + circle1->radius * (circle1->position.y - circle2->position.y) / intersectionDepth;
                circle2->position.x = midPoint.x + circle2->radius * (circle2->position.x - circle1->position.x) / intersectionDepth;
                circle2->position.y = midPoint.y + circle2->radius * (circle2->position.y - circle1->position.y) / intersectionDepth;
            }
            // Dyanamic-dynamic
            else {
                /*float j = (-(1 + (coRestitution)) * glm::dot(circle2->velocity - circle1->velocity, normal)) /
                    (((1 / circle1->mass) + (1 / circle2->mass)));

                circle1->SetVelocity(circle1->velocity - j * normal / circle1->mass);
                circle2->SetVelocity(circle2->velocity + j * normal / circle2->mass);*/

                circle1->ResolveCollision(circle2);
            }
            circle1 = nullptr;
            circle2 = nullptr;
            return true;

        }

    }
    circle1 = nullptr;
    circle2 = nullptr;
    return false;
}

bool PhysicsScene::Circle2Plane(PhysicsObject* obj1, PhysicsObject* obj2)
{
    Circle* circle = dynamic_cast<Circle*>(obj1);
    Plane* plane = dynamic_cast<Plane*>(obj2);
    //if we are successful then test for collision
    if (circle != nullptr && plane != nullptr)
    {
        float sphereToPlane = glm::dot((circle->position + circle->velocity * fixedDeltaTime), plane->normal) - plane->distanceToOrigin;

        float intersection = circle->radius - sphereToPlane;
        float velocityOutOfPlane = glm::dot(circle->velocity, plane->normal);
        if (intersection > 0 && velocityOutOfPlane < 0)
        {
            plane->ResolveCollision(circle);
            return true;
        }

    }

    return false;
}

bool PhysicsScene::Plane2Circle(PhysicsObject* obj1, PhysicsObject* obj2) {
    return Circle2Plane(obj2, obj1);
}
