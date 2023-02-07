#include "Rocket.h"

void Rocket::Update(PhysicsScene* scene, float deltaTime) {
	if (ship->mass <= 0) {
		return;
	}
	count += deltaTime;
	while (count >= fireRate) {
		Circle* exhaust;
		float removeAmount = ship->mass < fireAmount ? ship->mass : fireAmount;

		exhaust = new Circle(ship->position, glm::vec2(0, 0), removeAmount, 0.5f , 1.0f, glm::vec4(0, 1, 0, 1));
		scene->AddActor(exhaust);

		ship->mass -= removeAmount;
		ship->AddForceToActor(exhaust, force);

		count -= fireRate;
	}
}


Rocket::~Rocket() {
	delete ship;
	ship = nullptr;
}