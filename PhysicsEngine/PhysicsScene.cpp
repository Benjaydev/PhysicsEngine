#pragma once
#include "PhysicsScene.h"
#include "PhysicsEngine.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
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
    sceneActors.clear();

    for (PhysicsObject* uActor : UIActors)
    {
        delete uActor;
    }
    UIActors.clear();
}

void PhysicsScene::AddActor(PhysicsObject* actor)
{
    sceneActors.push_back(actor);
}
void PhysicsScene::AddUI(PhysicsObject* actor)
{
    UIActors.push_back(actor);
}
void PhysicsScene::RemoveActor(PhysicsObject* actor)
{
    auto iter = std::find(sceneActors.begin(), sceneActors.end(), actor);
    if (iter != sceneActors.end()) {
        sceneActors.erase(iter);
    }
}
void PhysicsScene::RemoveUI(PhysicsObject* actor)
{
    auto iter = std::find(UIActors.begin(), UIActors.end(), actor);
    if (iter != UIActors.end()) {
        UIActors.erase(iter);
    }
}
void PhysicsScene::QueueDestroy(PhysicsObject* actor)
{
    destroyQueue.push_back(actor);
}

void PhysicsScene::DestroyAllQueued()
{
    for (PhysicsObject* actor : destroyQueue) {
        RemoveActor(actor);
    }
}

void PhysicsScene::Draw()
{
    for (PhysicsObject* pActor : sceneActors) {
        pActor->Draw();
    }
    for (PhysicsObject* uActor : UIActors) {
        uActor->Draw();
    }
}


// function pointer array for doing our collisions
typedef bool(*CollisionFn)(PhysicsObject*, PhysicsObject*);


/*
column = 1st object
Row = 2nd object
            Plane Circle Box
    Plane
    Circle
    Box

*/
static CollisionFn collisionFunctionArray[] =
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
        spacePartition.clear();
        int gridCount = ((INT_MAX/2) - ((INT_MAX / 2) % gridSize)) / gridSize;
        for (PhysicsObject* pActor : sceneActors)
        {
            pActor->FixedUpdate(gravity, fixedDeltaTime);

            Rigidbody* body = dynamic_cast<Rigidbody*>(pActor);

            if (pActor->GetShapeID() == BOX) {
                //std::cout << body->position.x << ", " << body->position.y << std::endl;
            }

            if (body != nullptr) {
                glm::vec2 max = glm::vec2((INT_MAX / 2), (INT_MAX / 2));
                std::vector<glm::vec2> ps = { body->minBounds + max , body->maxBounds + max, glm::vec2(body->minBounds.x, body->maxBounds.y) + max, glm::vec2(body->minBounds.y, body->maxBounds.x) + max };

                int toGridx = ((ps[0].x - ((int)ps[0].x % gridSize)) / gridSize);
                int toGridy = ((ps[0].y - ((int)ps[0].y % gridSize)) / gridSize);

                int minGridPointx = toGridx;
                int minGridPointy = toGridx;
                int maxGridPointx = toGridy;
                int maxGridPointy = toGridy;


                // Get the range of grids that this object is overlapping
                for (int i = 0; i < 3; i++) {
                    // Convert to grid point
                    toGridx = ((ps[i].x - ((int)ps[i].x % gridSize)) / gridSize);
                    toGridy = ((ps[i].y - ((int)ps[i].y % gridSize)) / gridSize);

                    if (toGridx < minGridPointx) {
                        minGridPointx = toGridx;
                    }
                    else if (toGridx > maxGridPointx) {
                        maxGridPointx = toGridx;
                    }
                    if (toGridy < minGridPointy) {
                        minGridPointy = toGridy;
                    }
                    else if (toGridy > maxGridPointy) {
                        maxGridPointy = toGridy;
                    }
                }


                // Get every point in grid within range
                for (int y = minGridPointy; y < maxGridPointy+1; y++) {
                    for (int x = minGridPointx; x < maxGridPointx+1; x++) {

                        if (y == minGridPointy || x == minGridPointx || x == maxGridPointx || y == maxGridPointy) {
                            int pos = x + (y * gridCount);

                            // See if grid point already exists
                            auto it = spacePartition.find(pos);
                            if (it != spacePartition.end()) {
                                it->second.push_back(body);
                            }
                            // If not, create one
                            else {
                                std::vector<Rigidbody*> newBodies = std::vector<Rigidbody*>();
                                newBodies.push_back(body);
                                spacePartition.insert({ pos, newBodies });
                            }
                        }                  
                    }
                }


            }

        }
        for (PhysicsObject* uActor : UIActors)
        {
            uActor->FixedUpdate(gravity, fixedDeltaTime);
        }
        accumulatedTime -= fixedDeltaTime;


        CheckCollisions();

        DestroyAllQueued();
    }


    if (PhysicsEngine::configSettings["SHOULD_LERP_POSITIONS"] == 1) {
        // Do default update
        for (PhysicsObject* pActor : sceneActors)
        {
            pActor->Update(deltaTime);
        }
        for (PhysicsObject* uActor : UIActors)
        {
            uActor->Update(deltaTime);
        }
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


            // Only check planes like this
            if (shapeId1 < 0 || shapeId2 < 0 || (shapeId1 != PLANE && shapeId2 != PLANE)) {
                continue;
            }

            // using function pointers
            int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
            CollisionFn collisionFunctionPtr = collisionFunctionArray[functionIdx];
            if (collisionFunctionPtr != nullptr)
            {
                // did a collision occur?
                collisionFunctionPtr(object1, object2);
            }
        }
    }

    std::map<PhysicsObject*, std::vector<PhysicsObject*>> previousChecks;
    // Check space partition
    for (auto it = spacePartition.begin(); it != spacePartition.end(); ++it) {
        for (int outer = 0; outer < it->second.size(); outer++) {
            for (int inner = outer + 1; inner < it->second.size(); inner++) {
                PhysicsObject* object1 = it->second[outer];
                PhysicsObject* object2 = it->second[inner];

                int shapeId1 = object1->GetShapeID();
                int shapeId2 = object2->GetShapeID();


                // Only check planes like this
                if (shapeId1 < 0 || shapeId2 < 0) {
                    continue;
                }

                // using function pointers
                int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
                CollisionFn collisionFunctionPtr = collisionFunctionArray[functionIdx];
                if (collisionFunctionPtr != nullptr)
                {
                    //bool added = false;
                    //// Search all checks
                    //auto previous = previousChecks.find(object1);
                    //if (previous != previousChecks.end()) {
                    //    // If the object has collided with second object
                    //    auto exists = std::find(previous->second.begin(), previous->second.end(), object2);
                    //    // If it has already collided
                    //    if (exists != previous->second.end()) {
                    //        continue;
                    //    }
                    //    previous->second.push_back(object2);
                    //    added = true;
                    //}
                    //else {
                    //    previousChecks.insert({ object1, std::vector<PhysicsObject*>({object2}) });
                    //    added = true;
                    //}

                    //// Search all checks if last one was unsuccessful 
                    //if (!added) {
                    //    auto previous2 = previousChecks.find(object2);
                    //    if (previous2 != previousChecks.end()) {
                    //        // If the object has collided with second object
                    //        auto exists = std::find(previous2->second.begin(), previous2->second.end(), object1);
                    //        // If it has already collided
                    //        if (exists != previous2->second.end()) {
                    //            continue;
                    //        }
                    //        previous2->second.push_back(object1);
                    //    }
                    //    else {
                    //        previousChecks.insert({ object2, std::vector<PhysicsObject*>({object1}) });
                    //    }
                    //}


                    // did a collision occur?
                    collisionFunctionPtr(object1, object2);

                }
            }
        }
    }

}
PhysicsObject* PhysicsScene::CheckCollisionsOnPoint(glm::vec2 point, bool checkUI, bool checkWorldObjects)
{
    PhysicsObject* col = nullptr;
    std::vector<PhysicsObject*> allActors;

    if (checkUI) {
        col = CheckCollisionsOnPoint(point, UIActors);
    }
    if (col == nullptr && checkWorldObjects) {
        col = CheckCollisionsOnPoint(point, sceneActors);
    }
    return col;
}
PhysicsObject* PhysicsScene::CheckCollisionsOnPoint(glm::vec2 point, std::vector<PhysicsObject*> actors) {
    PhysicsObject* col = nullptr;
    for (PhysicsObject* actor : actors)
    {
        if (actor->GetShapeID() < 0) {
            continue;
        }
        else if (actor->GetShapeID() == BOX) {
            col = Point2Box(point, actor) ? actor : col;
        }
        else if (actor->GetShapeID() == CIRCLE) {
            col = Point2Circle(point, actor) ? actor : col;
        }
        else if (actor->GetShapeID() == PLANE) {
            col = Point2Plane(point, actor) ? actor : col;
        }
    }

    return col;
}


glm::vec2 PhysicsScene::NearestPointOnLine(glm::vec2 linePoint, glm::vec2 dir, glm::vec2 checkPoint)
{
    dir = glm::normalize(dir);
    glm::vec2 v = checkPoint - linePoint;
    float d = glm::dot(v, dir);
    return linePoint + dir * d;
}


void PhysicsScene::ApplyContactForces(Rigidbody* body1, Rigidbody* body2, glm::vec2 norm, float pen) {

    if ((body1 && body1->isTrigger) || (body2 && body2->isTrigger)) {
        return;
    }


    float body2Mass = body2 ? body2->GetMass() : INT_MAX;

    float body1Factor = body2Mass / (body1->GetMass() + body2Mass);

    body1->position = body1->position - body1Factor * norm * pen;
    if (body2) {
        body2->position = body2->position + (1 - body1Factor) * norm * pen;
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
        // Test for other collision
        glm::vec2 diff = (circle2->position) - (circle1->position);
        float sqrMag = glm::dot(diff, diff);
        float combRadi = (circle1->radius + circle2->radius);

        float intersectionDepth = sqrMag - (combRadi * combRadi);
        float dir = glm::dot((circle1->velocity - circle2->velocity), (circle1->position - circle2->position));

        if (intersectionDepth < 0 && dir < 0) {
            float coRestitution = PhysicsEngine::CalculateCoefficientRestitution(circle1->restitution, circle2->restitution);
            glm::vec2 staticVec = glm::vec2(0, 0);
            float mag = sqrt(sqrMag);
            glm::vec2 normal = diff / mag;

            float penetration = circle1->radius + circle2->radius - mag;
            if (penetration > 0)
            {
                circle1->ResolveCollision(circle2, 0.5f * (circle1->position + circle2->position), nullptr, penetration);
                circle1 = nullptr;
                circle2 = nullptr;
                return true;
            }


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
            glm::vec2 contact = circle->position - (plane->normal * circle->radius);
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
        float penetration = circle->radius - glm::length(circleToBox);

        
        if (penetration > 0)
        {
            glm::vec2 direction = circleToBox != glm::vec2(0) ? (circleToBox / penetration) : box->isKinematic ? -circle->velocity : -box->velocity;


            glm::vec2 contact = closestPointOnBoxWorld;
            box->ResolveCollision(circle, contact, &direction, penetration);
            return true;
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
            box1->ResolveCollision(box2, contact / (float)numContacts, &norm, pen);
        }
        return true;
    }

    return false;
}

bool PhysicsScene::Point2Plane(glm::vec2 point, PhysicsObject* obj) {
    return false;
}
bool PhysicsScene::Point2Box(glm::vec2 point, PhysicsObject* obj) {
    Box* box = dynamic_cast<Box*>(obj);
    if (box != nullptr)
    {
        // Rotate point to be relative to box orientation
        glm::vec2 diff = box->position - point;
        float radius = sqrt((diff.x * diff.x) + (diff.y * diff.y));
        float newAngle = atan2(point.y - box->position.y, point.x - box->position.x) + box->orientation;
        float newX = box->position.x + radius * cos(newAngle);
        float newY = box->position.y + radius * sin(newAngle);

        // If inside bounds
        return (newY > box->position.y - box->extents.y) && (newY < box->position.y + box->extents.y) && (newX > box->position.x - box->extents.x) && (newX < box->position.x + box->extents.x);
    }
}
bool PhysicsScene::Point2Circle(glm::vec2 point, PhysicsObject* obj) {
    Circle* circle = dynamic_cast<Circle*>(obj);
    glm::vec2 diff = (circle->position - point);
    float distSqr = glm::dot(diff, diff);

    if (distSqr < circle->radius * circle->radius) {
        return true;
    }
    return false;

}