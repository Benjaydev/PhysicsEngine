#pragma once
#include <map>
#include <sstream>
#include <fstream>
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

	static PhysicsEngine* physicsEngine;

	glm::vec2 GetScreenSpacePoint(float xWorld, float yWorld);
	glm::vec2 GetWorldSpacePoint(float xScreen, float yScreen);

	static float CalculateCoefficientRestitution(float restitution1, float restitution2);
	static float CoRestMult(float restitution1, float restitution2) { return restitution1 * restitution2; }
	static float CoRestMin(float restitution1, float restitution2) { return fmin(restitution1, restitution2); }
	static float CoRestMax(float restitution1, float restitution2) { return fmax(restitution1, restitution2); }

	static void LoadConfig();


	static std::map<std::string, int> configSettings;


protected:
	aie::Renderer2D*	m_2dRenderer = nullptr;
	aie::Font*			m_font = nullptr;
	PhysicsScene* physicsScene = nullptr;

	float aspectRatio = 0.f;
	glm::vec2 orthoRange = glm::vec2(-100, 100);
};