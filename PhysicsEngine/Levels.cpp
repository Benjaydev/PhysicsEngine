#pragma once
#include "Levels.h"
#include <iostream>
#include "SoftBody.h"



void Levels::SaveToConsole(std::vector<PhysicsObject*> actors) {
	int i = 0;


	for (PhysicsObject* actor : actors) {
		i++;
		if (actor->GetShapeID() == CIRCLE) {
			Circle* body = dynamic_cast<Circle*>(actor);
			std::cout << "Circle* " << "actor" << i << " = new Circle(glm::vec2(" << body->position.x << ", " << body->position.y <<
				"), glm::vec2(0), " <<
				body->mass <<
				", " << body->radius <<
				", " << body->restitution <<
				", " << "glm::vec4(" << body->colour.r << ", " << body->colour.g << ", " << body->colour.b << ", " << body->colour.a << "));" << std::endl;
		}
		if (actor->GetShapeID() == BOX) {
			Box* body = dynamic_cast<Box*>(actor);
			std::cout << "Box* " << "actor" << i << " = new Box(glm::vec2(" << body->position.x << ", " << body->position.y <<
				"), glm::vec2(" << body->extents.x << ", " << body->extents.y <<
				"), glm::vec2(0), " <<
				body->mass <<
				", " << body->restitution <<
				", " << "glm::vec4(" << body->colour.r << ", " << body->colour.g << ", " << body->colour.b << ", " << body->colour.a << "));" << std::endl;
		}
		if (actor->GetShapeID() == PLANE) {
			Plane* body = dynamic_cast<Plane*>(actor);
			std::cout << "Plane* " << "actor" << i << " = new Plane(glm::vec2(" << body->normal.x << ", " << body->normal.y << "), " <<
				body->distanceToOrigin <<
				", " << body->restitution <<
				", " << "glm::vec4(" << body->colour.r << ", " << body->colour.g << ", " << body->colour.b << ", " << body->colour.a << "));" << std::endl;
		}
		Rigidbody* body = dynamic_cast<Rigidbody*>(actor);
		if (body && body->isKinematic) {
			std::cout << "actor" << i << "->isKinematic=true;" << std::endl;
		}

		std::cout << "scene->AddActor(actor" << i << ");" << std::endl;
	}
}


void Levels::Level1(PhysicsScene* scene) {

	scene->gravity = glm::vec2(0, -982.0f);

	Plane* actor1 = new Plane(glm::vec2(0, 1), -30, 0, glm::vec4(0.5, 0, 1, 1));
	scene->AddActor(actor1);
	Plane* actor2 = new Plane(glm::vec2(0.657498, 0.753457), -314.703, 0.5, glm::vec4(0.5, 0, 1, 1));
	scene->AddActor(actor2);

	Plane* actor3 = new Plane(glm::vec2(-0.657498, 0.753457), -314.703, 0.5, glm::vec4(0.5, 0, 1, 1));
	scene->AddActor(actor3);
	//Box* actor3 = new Box(glm::vec2(1702.78, 25.4881), glm::vec2(499.636, 53.9772), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor3->isKinematic = true;
	//scene->AddActor(actor3);
	//Box* actor4 = new Box(glm::vec2(1295.09, 201.861), glm::vec2(48.1274, 121.68), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor4->isKinematic = true;
	//scene->AddActor(actor4);
	//Box* actor5 = new Box(glm::vec2(1706.44, 378.853), glm::vec2(570.264, 53.5757), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor5->isKinematic = true;
	//scene->AddActor(actor5);
	//Box* actor6 = new Box(glm::vec2(2081.47, 201.861), glm::vec2(44.4951, 119.864), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor6->isKinematic = true;
	//scene->AddActor(actor6);
	//Box* actor7 = new Box(glm::vec2(1451.13, 533.713), glm::vec2(39.5552, 99.9868), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor7->isKinematic = true;
	//scene->AddActor(actor7);
	//Box* actor8 = new Box(glm::vec2(1719.22, 694.131), glm::vec2(380.17, 58.2341), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor8->isKinematic = true;
	//scene->AddActor(actor8);
	//Box* actor9 = new Box(glm::vec2(2009.3, 535.91), glm::vec2(37.3577, 99.9868), glm::vec2(0), 90, 0, glm::vec4(1, 0, 0, 1));
	////actor9->isKinematic = true;
	//scene->AddActor(actor9);
	//Circle* actor10 = new Circle(glm::vec2(1740.1, 501.929), glm::vec2(0), 10, 69.5002, 0, glm::vec4(0, 1, 0, 1));
	////actor10->isKinematic = true;
	//scene->AddActor(actor10);
	//Circle* actor11 = new Circle(glm::vec2(1410.47, 820.488), glm::vec2(0), 10, 63.7374, 0, glm::vec4(0, 1, 0, 1));
	////actor11->isKinematic = true;
	//scene->AddActor(actor11);
	//Circle* actor12 = new Circle(glm::vec2(1446.73, 139.259), glm::vec2(0), 10, 55.1133, 0, glm::vec4(0, 1, 0, 1));
	////actor12->isKinematic = true;
	//scene->AddActor(actor12);
	//Circle* actor13 = new Circle(glm::vec2(2232.34, 508.441), glm::vec2(0), 10, 73.4525, 0, glm::vec4(0, 1, 0, 1));
	////actor13->isKinematic = true;
	//scene->AddActor(actor13);


	/*Box* ui = new Box(glm::vec2(0,0), glm::vec2(100, 100), glm::vec2(0), 90, 0, glm::vec4(1, 1, 0, 1));
	ui->isKinematic = true;
	scene->AddUI(ui);*/

	/*Circle* ui = new Circle(glm::vec2(0,0), glm::vec2(0), 10, 73.4525, 0, glm::vec4(0, 1, 0, 1));
	ui->isKinematic = true;
	scene->AddUI(ui);*/

	/*actor3->collisionCallback = [=](PhysicsObject* other) { actor3->isKinematic = false; };
	actor4->collisionCallback = [=](PhysicsObject* other) { actor4->isKinematic = false; };
	actor5->collisionCallback = [=](PhysicsObject* other) { actor5->isKinematic = false; };
	actor6->collisionCallback = [=](PhysicsObject* other) { actor6->isKinematic = false; };
	actor7->collisionCallback = [=](PhysicsObject* other) { actor7->isKinematic = false; };
	actor8->collisionCallback = [=](PhysicsObject* other) { actor8->isKinematic = false; };
	actor9->collisionCallback = [=](PhysicsObject* other) { actor9->isKinematic = false; };
	actor10->collisionCallback = [=](PhysicsObject* other) { actor10->isKinematic = false; };
	actor11->collisionCallback = [=](PhysicsObject* other) { actor11->isKinematic = false; };
	actor12->collisionCallback = [=](PhysicsObject* other) { actor12->isKinematic = false; };
	actor13->collisionCallback = [=](PhysicsObject* other) { actor13->isKinematic = false; };*/


	//Circle* circle = new Circle(glm::vec2(0, 0), glm::vec2(0), 10.f, 10.f, 1.f, glm::vec4(1, 1, 1, 1));
	//circle->isTrigger = true;
	//circle->isKinematic = true;
	//scene->AddActor(circle);

	//circle->triggerEnterCallback = [=](PhysicsObject* other) { std::cout << "Enter:" <<
	//	other << std::endl; };
	//circle->triggerExitCallback = [=](PhysicsObject* other) { std::cout << "Exit:" <<
	//	other << std::endl; };




	//physicsScene->AddActor(plane4);


	/*std::vector<std::string> sb;
	sb.push_back("000000");
	sb.push_back("000000");
	sb.push_back("000000");
	sb.push_back("000000");


	SoftBody::Build(scene, glm::vec2(0, 200), 250.0f, 0, 1, sb, 0.1f, 20);*/
}