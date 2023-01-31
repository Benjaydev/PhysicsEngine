#include "Plane.h"

Plane::Plane() : 
    PhysicsObject(PLANE) {
    distanceToOrigin = 0;
    normal = glm::vec2(0, 1);
    colour = glm::vec4(1, 1, 1, 1);
}

Plane::Plane(glm::vec2 _normal, float _distance, glm::vec4 _colour) : 
    PhysicsObject(PLANE) {
    distanceToOrigin = _distance;
    normal = _normal;
    colour = _colour;
}


void Plane::Draw() {
    float lineSegmentLength = 300;
    glm::vec2 centerPoint = normal * distanceToOrigin;
    // easy to rotate normal through 90 degrees around z
    glm::vec2 parallel(normal.y, -normal.x);
    glm::vec4 colourFade = colour;
    colourFade.a = 0;
    glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
    glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
    //aie::Gizmos::add2DLine(start, end, colour);
    aie::Gizmos::add2DTri(start, end, start - normal * 10.0f, colour, colour, colourFade);
    aie::Gizmos::add2DTri(end, end - normal * 10.0f, start - normal * 10.0f, colour, colourFade, colourFade);
}