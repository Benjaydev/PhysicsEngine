#include "Box.h"

Box::Box(glm::vec2 _position, glm::vec2 _extents, glm::vec2 _velocity, float _mass, float _restitution, glm::vec4 _colour) :
	Rigidbody(BOX, _position, _velocity, 0, _mass, _restitution) {
	colour = _colour;
	extents = _extents;

}


void Box::FixedUpdate(glm::vec2 gravity, float timeStep) {
	Rigidbody::FixedUpdate(gravity, timeStep);
	// Calculate moment of inertia
	moment = (1.f / 12.f) * mass * GetWidth() * GetHeight();

}

void Box::Update(float deltaTime) {
	Rigidbody::Update(deltaTime);
}

void Box::Draw() {
	// draw using local axes 
	glm::vec2 p1 = visualPosition - visualLocalX * extents.x - visualLocalY * extents.y;
	glm::vec2 p2 = visualPosition + visualLocalX * extents.x - visualLocalY * extents.y;
	glm::vec2 p3 = visualPosition - visualLocalX * extents.x + visualLocalY * extents.y;
	glm::vec2 p4 = visualPosition + visualLocalX * extents.x + visualLocalY * extents.y;
	aie::Gizmos::add2DTri(p1, p2, p4, colour);
	aie::Gizmos::add2DTri(p1, p4, p3, colour);


	Rigidbody::Draw();
}


// Check if any of the other box's corners are inside this box 
bool Box::CheckBoxCorners(const Box& box, glm::vec2& contact, int& numContacts, float& pen, glm::vec2& edgeNormal)
{
	float minX, maxX, minY, maxY;
	float boxW = box.extents.x * 2;
	float boxH = box.extents.y * 2;
	int numLocalContacts = 0;
	glm::vec2 localContact(0, 0);
	bool first = true;

	// Loop over all corners of the other box 
	for (float x = -box.extents.x; x < boxW; x += boxW)
	{
		for (float y = -box.extents.y; y < boxH; y += boxH)
		{
			// Get the position in worldspace 
			glm::vec2 p = box.position + x * box.localX + y * box.localY;
			// Get the position in our box's space 
			glm::vec2 p0(glm::dot(p - position, localX), glm::dot(p - position, localY));

			// Update the extents in each cardinal direction in our box's space 
			// (ie extents along the separating axes) 
			if (first || p0.x < minX) minX = p0.x;
			if (first || p0.x > maxX) maxX = p0.x;
			if (first || p0.y < minY) minY = p0.y;
			if (first || p0.y > maxY) maxY = p0.y;

			// If this corner is inside the box, add it to the list of contact points 
			if (p0.x >= -extents.x && p0.x <= extents.x && p0.y >= -extents.y && p0.y <= extents.y)
			{
				numLocalContacts++;
				localContact += p0;
			}
			first = false;
		}
	}

	// If it lie entirely to one side of the box along one axis, we've found a separating
	// axis, and we can exit 
	if (maxX <= -extents.x || minX >= extents.x || maxY <= -extents.y || minY >= extents.y)
		return false;

	if (numLocalContacts == 0)
		return false;

	bool res = false;
	contact += position + (localContact.x * localX + localContact.y * localY) / (float)numLocalContacts;
	numContacts++;

	// Find the minimum penetration vector as a penetration amount and normal 
	float pen0 = extents.x - minX;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = localX;
		pen = pen0;
		res = true;
	}
	pen0 = maxX + extents.x;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -localX;
		pen = pen0;
		res = true;
	}
	pen0 = extents.y - minY;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = localY;
		pen = pen0;
		res = true;
	}
	pen0 = maxY + extents.y;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -localY;
		pen = pen0;
		res = true;
	}
	return res;
}