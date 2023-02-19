#pragma once
#include <map>
#include <sstream>
#include <fstream>
#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"


class PhysicsEngine : public aie::Application {
public:

	PhysicsEngine();
	virtual ~PhysicsEngine();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void TestModeUpdate(float deltaTime);
	virtual void draw();

	static PhysicsEngine* physicsEngine;

	glm::vec2 GetScreenSpacePoint(float xWorld, float yWorld);
	glm::vec2 GetWorldSpacePoint(float xScreen, float yScreen);

	static float CalculateCoefficientRestitution(float restitution1, float restitution2);
	static float CalculateCoefficientStaticFriction(float restitution1, float restitution2);
	static float CoRestMult(float restitution1, float restitution2) { return restitution1 * restitution2; }
	static float CoRestAve(float restitution1, float restitution2) { return (restitution1 + restitution2)/2.0f; }
	static float CoRestMin(float restitution1, float restitution2) { return fmin(restitution1, restitution2); }
	static float CoRestMax(float restitution1, float restitution2) { return fmax(restitution1, restitution2); }

	static void LoadConfig();

	static std::string Encrypt(std::string text, std::string key);
	static std::string Decrypt(std::string text, std::string key);


	void Rope(int num, glm::vec2 position, bool isKinetic = true);

	static std::map<std::string, int> configSettings;
	static std::map<std::string, float> configValues;

	static void DrawText(const char* text, glm::vec2 worldPos);

	float spawnSize = 1.f;
	int ropeSize = 1;

	glm::vec2 dragStart = glm::vec2(0);
	glm::vec2 dragEnd = glm::vec2(0);

	glm::vec2 middleMouseDragStart = glm::vec2(0);
	glm::vec2 middleMouseDragDist = glm::vec2(0);
	double lastScrollValue = 0.0f;

	bool middleMouseDrag = false;
	bool circleDrag = false;
	bool boxDrag = false;
	bool planeDrag = false;
	bool planeClockwise = true;
	bool shouldErase = false;

	PhysicsObject* selectedObject = nullptr;

	float orthoSize = 1000;
	const float maxOrthoSize =  10000000;
	glm::vec2 orthoCenter = glm::vec2(0, 0);



	PhysicsScene* physicsScene = nullptr;

protected:
	aie::Renderer2D* m_2dRenderer = nullptr;
	aie::Font* m_font = nullptr;


	float aspectRatio = 0.f;

};