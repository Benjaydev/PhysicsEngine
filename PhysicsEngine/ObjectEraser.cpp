#include "ObjectEraser.h"

ObjectEraser::ObjectEraser(glm::vec2 _position, glm::vec2 _extents, glm::vec2 _velocity, float _mass, float _restitution, glm::vec4 _colour) :
	Box(_position, _extents, _velocity, _mass, _restitution, _colour) {
	colour = _colour;
	extents = _extents;
	isKinematic = true;
	shapeID = ERASER;
}