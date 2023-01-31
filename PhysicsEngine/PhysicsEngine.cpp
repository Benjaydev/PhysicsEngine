#pragma once
#include "PhysicsEngine.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"
#include <glm/ext.hpp>


PhysicsEngine::PhysicsEngine() {

}

PhysicsEngine::~PhysicsEngine() {

}

bool PhysicsEngine::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	physicsScene = new PhysicsScene();
	physicsScene->SetGravity(glm::vec2(0, -9.8f));
	physicsScene->SetFixedDeltaTime(0.01f);

	aie::Gizmos::create(0, 0, 100, 2000);


	Circle* circle;
	Circle* circle2;
	circle = new Circle(glm::vec2(-40, 0), glm::vec2(10, 30), 90.0f, 10.0f, 1.0f, glm::vec4(1, 0, 0, 1));
	circle2 = new Circle(glm::vec2(40, 0), glm::vec2(-10, 30), 90.0f, 10.0f, 1.0f, glm::vec4(0, 1, 0, 1));
	physicsScene->AddActor(circle2);
	physicsScene->AddActor(circle);


	Circle* ball1 = new Circle(glm::vec2(-20, 0), glm::vec2(0), 4.0f, 4, 0.3f, glm::vec4(1, 0, 0, 1));
	//Circle* ball2 = new Circle(glm::vec2(10, 0), glm::vec2(0), 4.0f, 4, glm::vec4(0, 1, 0, 1));
	Plane* plane = new Plane(glm::vec2(0, 1), -30, glm::vec4(1, 1, 1, 1));

	physicsScene->AddActor(ball1);
	//physicsScene->AddActor(ball2);
	physicsScene->AddActor(plane);


	return true;
}

void PhysicsEngine::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PhysicsEngine::update(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();

	aie::Gizmos::clear();

	if (physicsScene != nullptr) {
		physicsScene->Update(deltaTime);
		physicsScene->Draw();
	}




	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void PhysicsEngine::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	static float aspectRatio = 16 / 9.f;
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));

	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);
	// done drawing sprites
	m_2dRenderer->end();
}