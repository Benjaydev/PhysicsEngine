#pragma once
#include "PhysicsEngine.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"
#include <glm/ext.hpp>


PhysicsEngine* PhysicsEngine::physicsEngine = nullptr;

std::map<std::string, int> PhysicsEngine::configSettings;
std::map<std::string, float> PhysicsEngine::configValues;

// Coefficient calculation techniques
typedef float(*coRest)(float, float);
static coRest CoefficientCalculationFunctions[] = {
	PhysicsEngine::CoRestMult, PhysicsEngine::CoRestAve, PhysicsEngine::CoRestMin, PhysicsEngine::CoRestMax
};

PhysicsEngine::PhysicsEngine() {

}

PhysicsEngine::~PhysicsEngine() {

}


bool PhysicsEngine::startup() {
	

	physicsEngine = this;


	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	physicsScene = new PhysicsScene();
	physicsScene->gravity = glm::vec2(0, -9.8f);
	physicsScene->SetFixedDeltaTime(0.01f);

	aie::Gizmos::create(0, 0, 100, 2000);


	//Circle* circle;
	//Circle* circle2;
	//circle = new Circle(glm::vec2(0, 0), glm::vec2(10, 30), 90.0f, 10.0f, 0.5f, glm::vec4(1, 0, 0, 1));
	//circle2 = new Circle(glm::vec2(40, 0), glm::vec2(-10, 10), 90.0f, 10.0f, 1.0f, glm::vec4(0, 1, 0, 1));
	//physicsScene->AddActor(circle);
	//physicsScene->AddActor(circle2);
	//physicsScene->AddActor(circle);

	//Box* box = new Box(glm::vec2(0, 0), glm::vec2(10, 5), glm::vec2(0, 0), 90.0f, 0.5f, glm::vec4(1, 0, 0, 1));
	//box->angularVelocity = 2.f;
	//physicsScene->AddActor(box);


	//Circle* ball1 = new Circle(glm::vec2(-20, 0), glm::vec2(0,0), 4.0f, 4, 0.3f, glm::vec4(1, 0, 0, 1));
	//Circle* ball2 = new Circle(glm::vec2(10, 0), glm::vec2(0), 4.0f, 4, glm::vec4(0, 1, 0, 1));
	Plane* plane = new Plane(glm::vec2(0, 1), -30, glm::vec4(1, 1, 1, 1));
	Plane* plane2 = new Plane(glm::vec2(1, 0), -90, glm::vec4(1, 1, 1, 1));
	Plane* plane3 = new Plane(glm::vec2(-1, 0), -90, glm::vec4(1, 1, 1, 1));
	Plane* plane4 = new Plane(glm::vec2(0.30f, 0.70f), -40, glm::vec4(1, 1, 1, 1));

	//physicsScene->AddActor(ball1);
	//physicsScene->AddActor(ball2);
	physicsScene->AddActor(plane);
	physicsScene->AddActor(plane2);
	physicsScene->AddActor(plane3);
	physicsScene->AddActor(plane4);

	/*for (int i = 0; i < 10; i++) {
		Circle* circle = new Circle(glm::vec2(20 -i*10, 10), glm::vec2(0, 0), 90.0f, 5, 1.0f, glm::vec4(1,0,0, 1));
		physicsScene->AddActor(circle);
	}*/

	LoadConfig();
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


	if (input->wasKeyPressed(aie::INPUT_KEY_C)) {
		int x = 0;
		int y = 0;
			
		input->getMouseXY(&x, &y);

		glm::vec2 worldPos = GetWorldSpacePoint(x, y);
		glm::vec2 screenPos = GetScreenSpacePoint(worldPos.x, worldPos.y);
		Circle* circle = new Circle(glm::vec2(worldPos.x, worldPos.y), glm::vec2(0, 0), 10.0f, 10.0f, 1.0f, glm::vec4(0, 1, 0, 1));
		physicsScene->AddActor(circle);
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_S)) {
		int x = 0;
		int y = 0;

		input->getMouseXY(&x, &y);

		glm::vec2 worldPos = GetWorldSpacePoint(x, y);
		glm::vec2 screenPos = GetScreenSpacePoint(worldPos.x, worldPos.y);
		Box* box = new Box(glm::vec2(worldPos.x, worldPos.y), glm::vec2(10, 5), glm::vec2(0, 0), 90.0f, 1.0f, glm::vec4(1, 0, 0, 1));

		physicsScene->AddActor(box);
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
	aspectRatio = 16 / 9.f;
	aie::Gizmos::draw2D(glm::ortho<float>(orthoRange.x, orthoRange.y, orthoRange.x / aspectRatio, orthoRange.y / aspectRatio, -1.0f, 1.0f));

	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);
	// done drawing sprites
	m_2dRenderer->end();
}


glm::vec2 PhysicsEngine::GetScreenSpacePoint(float xWorld, float yWorld) {
	int x = ((xWorld + orthoRange.y) / (orthoRange.y * 2.f)) * getWindowWidth();
	int y = ((yWorld + orthoRange.y / aspectRatio) / (orthoRange.y / aspectRatio * 2.f)) * getWindowHeight();

	return glm::vec2(x, y);
}

glm::vec2 PhysicsEngine::GetWorldSpacePoint(float xScreen, float yScreen) {
	// AIE bootstrap uses the ortho value ranges for drawing things on the screen. So convert screen point to ortho point
	float x = ((xScreen / getWindowWidth()) * orthoRange.y * 2) + orthoRange.x;
	float y = (((yScreen / getWindowHeight()) * orthoRange.y * 2) + orthoRange.x) / aspectRatio;
	return glm::vec2(x, y);
}
void PhysicsEngine::LoadConfig() {
	std::ifstream is_file("config.txt");

	if (is_file) {
		std::string line;
		while (std::getline(is_file, line))
		{
			std::istringstream iss(line);

			std::string key;
			if (std::getline(iss, key, '='))
			{
				std::string value;
				if (std::getline(iss, value))
				{
					// Value setting
					if (key.find("V_") != std::string::npos) {
						// Remove prefix from key
						key.erase(0, 2);
						float v = std::stof(value);
						configValues.emplace(key, v);
					}
					// Int setting
					else {
						int i = std::stoi(value);
						configSettings.emplace(key, i);
					}

				}
			}
		}
	}
}



float PhysicsEngine::CalculateCoefficientRestitution(float restitution1, float restitution2) {

	return CoefficientCalculationFunctions[configSettings["RESTITUTION_CALC"]](restitution1, restitution2);
}

float PhysicsEngine::CalculateCoefficientStaticFriction(float restitution1, float restitution2) {

	return CoefficientCalculationFunctions[configSettings["STATFRICTION_CALC"]](restitution1, restitution2);
}

