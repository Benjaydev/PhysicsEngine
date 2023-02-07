#include "Box.h"

class ObjectEraser : public Box
{
public:
	ObjectEraser(glm::vec2 _position, glm::vec2 _extents, glm::vec2 _velocity,
		float _mass, float _restitution, glm::vec4 _colour);
	~ObjectEraser();

	void FixedUpdate(glm::vec2 gravity, float timeStep) override;
	void Draw() override;

	void ResolveCollision(Rigidbody* actor2, glm::vec2 contact, glm::vec2* collisionNormal = nullptr, float pen = 0) override;

};