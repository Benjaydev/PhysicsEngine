#include "Rigidbody.h"

class Box : public Rigidbody
{
public:
	Box(glm::vec2 _position, glm::vec2 _extents, glm::vec2 _velocity,
		float _mass, float _restitution, glm::vec4 _colour);
	~Box();

	void FixedUpdate(glm::vec2 gravity, float timeStep) override;
	void Update(float deltaTime) override;
	void Draw() override;

	float GetWidth() { return extents.x * 2; }
	float GetHeight() { return extents.y * 2; }

	bool CheckBoxCorners(const Box& box, glm::vec2& contact, int& numContacts, float& pen, glm::vec2& edgeNormal);

	// The halfedge lengths  
	glm::vec2 extents; 
	glm::vec4 colour;



};