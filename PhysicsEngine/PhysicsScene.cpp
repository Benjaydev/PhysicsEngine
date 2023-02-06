#pragma once
#include "PhysicsScene.h"
#include "PhysicsEngine.h"

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
    PhysicsScene::Plane2Plane,  PhysicsScene::Plane2Circle, PhysicsScene::Plane2Box,
    PhysicsScene::Circle2Plane, PhysicsScene::Circle2Circle, PhysicsScene::Circle2Box,
    PhysicsScene::Box2Plane, PhysicsScene::Box2Circle, PhysicsScene::Box2Box
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
        glm::vec2 diff = (circle2->position) - (circle1->position);
        float sqrMag = glm::dot(diff, diff);
        float combRadi = (circle1->radius + circle2->radius);

        float intersectionDepth = sqrMag - (combRadi * combRadi);
        float dir = glm::dot((circle1->velocity - circle2->velocity), (circle1->position - circle2->position));
        if (intersectionDepth < 0 && dir < 0) {
            float coRestitution = PhysicsEngine::CalculateCoefficientRestitution(circle1->restitution, circle2->restitution);
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
                circle1->ResolveCollision(circle2, 0.5f * (circle1->position + circle2->position));
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
        float sphereToPlane = glm::dot((circle->position), plane->normal) - plane->distanceToOrigin;

        float intersection = circle->radius - sphereToPlane;
        float velocityOutOfPlane = glm::dot(circle->velocity, plane->normal);
        if (intersection > 0 && velocityOutOfPlane < 0)
        {
            glm::vec2 contact = circle->position + (plane->normal * circle->radius);
            plane->ResolveCollision(circle, contact);
            return true;
        }

    }

    return false;
}

bool PhysicsScene::Plane2Circle(PhysicsObject* obj1, PhysicsObject* obj2) {
    return Circle2Plane(obj2, obj1);
}



bool PhysicsScene::Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2)
{
    Plane* plane = dynamic_cast<Plane*>(obj1);
    Box* box = dynamic_cast<Box*>(obj2);

    //if we are successful then test for collision 
    if (box != nullptr && plane != nullptr)
    {
        int numContacts = 0;
        glm::vec2 contact(0, 0);
        float contactV = 0;

        // get a representative point on the plane 
        glm::vec2 planeOrigin = plane->normal * plane->distanceToOrigin;

        // check all four corners to see if we've hit the plane 
        for (float x = -box->extents.x; x < box->GetWidth(); x += box->GetWidth())
        {
            for (float y = -box->extents.y; y < box->GetHeight(); y += box->GetHeight())
            {
                // get the position of the corner in world space 
                glm::vec2 p = (box->position) + x * box->localX + y * box->localY;
                float distFromPlane = glm::dot(p - planeOrigin, plane->normal);

                // this is the total velocity of the point in world space 
                glm::vec2 displacement = x * box->localX + y * box->localY;
                glm::vec2 pointVelocity = box->velocity + box->angularVelocity * glm::vec2(-displacement.y, displacement.x);
                // and this is the component of the point velocity into the plane 
                float velocityIntoPlane = glm::dot(pointVelocity, plane->normal);

                // and moving further in, we need to resolve the collision 
                if (distFromPlane < 0 && velocityIntoPlane <= 0)
                {
                    numContacts++;
                    contact += p;
                    contactV += velocityIntoPlane;
                }
            }
        }

        // we've had a hit - typically only two corners can contact 
        if (numContacts > 0)
        {
            plane->ResolveCollision(box, contact / (float)numContacts);
            return true;
        }
    }

    return false;
}

bool PhysicsScene::Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2) {
    return Plane2Box(obj2, obj1);
}

bool PhysicsScene::Box2Circle(PhysicsObject* obj1, PhysicsObject* obj2) {
    Box* box = dynamic_cast<Box*>(obj1);
    Circle* circle = dynamic_cast<Circle*>(obj2);

    if (box != nullptr && circle != nullptr)
    {
        // Transform the circle into the box's coordinate space 
        glm::vec2 circlePosWorld = circle->position - box->position;
        glm::vec2 circlePosBox = glm::vec2(glm::dot(circlePosWorld, box->localX), glm::dot(circlePosWorld, box->localY));

        // Find the closest point to the circle centre on the box by clamping the coordinates in box - space to the box's extents 
        glm::vec2 closestPointOnBoxBox = circlePosBox;
        glm::vec2 extents = box->extents;
        if (closestPointOnBoxBox.x < -extents.x) {
            closestPointOnBoxBox.x = -extents.x;
        }
        if (closestPointOnBoxBox.x > extents.x) {
            closestPointOnBoxBox.x = extents.x;
        }
        if (closestPointOnBoxBox.y < -extents.y) {
            closestPointOnBoxBox.y = -extents.y;
        }
        if (closestPointOnBoxBox.y > extents.y) {
            closestPointOnBoxBox.y = extents.y;
        }
        // Convert back into world coordinates 
        glm::vec2 closestPointOnBoxWorld = box->position + closestPointOnBoxBox.x * box->localX + closestPointOnBoxBox.y * box->localY;
        glm::vec2 circleToBox = circle->position - closestPointOnBoxWorld;
        if (glm::length(circleToBox) < circle->radius)
        {
            glm::vec2 direction = glm::normalize(circleToBox);
            glm::vec2 contact = closestPointOnBoxWorld;
            box->ResolveCollision(circle, contact, &direction);
        }
    }

    return false;
}

bool PhysicsScene::Circle2Box(PhysicsObject* obj1, PhysicsObject* obj2) {
    return Box2Circle(obj2, obj1);

}bool PhysicsScene::Box2Box(PhysicsObject* obj1, PhysicsObject* obj2) {
    Box* box1 = dynamic_cast<Box*>(obj1);
    Box* box2 = dynamic_cast<Box*>(obj2);
    if (box1 != nullptr && box2 != nullptr) {
        glm::vec2 boxPos = box2->position - box1->position;
        glm::vec2 norm(0, 0);
        glm::vec2 contact(0, 0);
        float pen = 0;
        int numContacts = 0;
        box1->CheckBoxCorners(*box2, contact, numContacts, pen, norm);
        if (box2->CheckBoxCorners(*box1, contact, numContacts, pen, norm)) {
            norm = -norm;
        }
        if (pen > 0) {
            box1->ResolveCollision(box2, contact / (float)numContacts, &norm);
        }
        return true;
    }

    return false;
}
