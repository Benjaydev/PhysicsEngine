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

	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetTimeStep(0.01f);

	aie::Gizmos::create(0, 0, 100, 2000);

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

	if (m_physicsScene != nullptr) {
		m_physicsScene->Update(deltaTime);
		m_physicsScene->Draw();
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