#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"
#include "Rocket.h"

class PhysicsEngine : public aie::Application {
public:

	PhysicsEngine();
	virtual ~PhysicsEngine();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:
	aie::Renderer2D*	m_2dRenderer = nullptr;
	aie::Font*			m_font = nullptr;
	PhysicsScene* physicsScene = nullptr;
};