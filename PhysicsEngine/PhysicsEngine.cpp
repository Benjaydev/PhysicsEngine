#pragma once
#include "PhysicsEngine.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Gizmos.h"
#include <glm/ext.hpp>
#include "Spring.h"
#include "SoftBody.h"
#include <iostream>
#include <limits>
#include "Levels.h"

PhysicsEngine* PhysicsEngine::physicsEngine = nullptr;

std::map<std::string, int> PhysicsEngine::configSettings;
std::map<std::string, float> PhysicsEngine::configValues;

// Coefficient calculation techniques
typedef float(*coRest)(float, float);
static coRest CoefficientCalculationFunctions[] = {
	PhysicsEngine::CoRestMult, PhysicsEngine::CoRestAve, PhysicsEngine::CoRestMin, PhysicsEngine::CoRestMax
};

PhysicsEngine::PhysicsEngine() {
	LoadConfig();
}

PhysicsEngine::~PhysicsEngine() {
	m_font = nullptr;
	delete physicsScene;
	physicsScene = nullptr;
	physicsEngine = nullptr;
}


bool PhysicsEngine::startup() {

	physicsEngine = this;


	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("./font/consolas.ttf", 30);

	aie::Gizmos::create(0, 0, configValues["MAX_2D_LINES"], configValues["MAX_2D_TRIS"]);

	physicsScene = new PhysicsScene();
	Levels* levels = new Levels();
	levels->Level1(physicsScene);

	return true;
}

void PhysicsEngine::DrawText(const char* text, glm::vec2 worldPos) {
	physicsEngine->m_2dRenderer->begin();
	glm::vec2 screenPos = physicsEngine->GetScreenSpacePoint(worldPos.x, worldPos.y);
	physicsEngine->m_2dRenderer->drawText(physicsEngine->m_font, text, screenPos.x, screenPos.y);
	physicsEngine->m_2dRenderer->end();
}
 
void PhysicsEngine::Rope(int num, glm::vec2 position, bool isKinetic) {

	Circle* prev = nullptr;
	for (int i = 0; i < num; i++)
	{
		// spawn a sphere to the right and below the previous one, so that the whole rope acts under gravityand swings
		Circle* circle = new Circle(glm::vec2(position.x, position.y - i * 5), glm::vec2(0), 10.f, 2.f, 1.f, glm::vec4(1,0, 0, 1));
		if (i == 0)
			circle->isKinematic = isKinetic;
		physicsScene->AddActor(circle);
		if (prev)
			physicsScene->AddActor(new Spring(circle, prev, 500, 10, 7));
		prev = circle;
	}
}


void PhysicsEngine::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PhysicsEngine::update(float deltaTime) {

	physicsScene->SetFixedDeltaTime(configValues["FIXED_UPDATE"]);

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

	if (configSettings["TEST_MODE"] == 1) {
		TestModeUpdate(deltaTime);
	}

	if (input->isKeyDown(aie::INPUT_KEY_L)) {
		for (int i = 0; i < 1; i++) {
			Circle* circle = new Circle(glm::vec2(0, 500), glm::vec2(1000, -100), 10.0f, fmax(0.1f, 10), 0.5f, shouldErase ? glm::vec4(0) : glm::vec4(0, 1, 0, 1));
			physicsScene->AddActor(circle);
		}



	}
	std::cout << physicsScene->spacePartition.size() << std::endl;

}
void PhysicsEngine::TestModeUpdate(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();



	if (input->wasKeyPressed(aie::INPUT_KEY_O)) {
		Levels::SaveToConsole(physicsScene->GetSceneActors());
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_R)) {
		LoadConfig();
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_UP)) {
		ropeSize++;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_DOWN)) {
		ropeSize--;
	}

	int x = 0;
	int y = 0;

	input->getMouseXY(&x, &y);

	glm::vec2 worldPos = GetWorldSpacePoint(x, y);


	if (input->wasMouseButtonPressed(0)) {
		PhysicsObject* col = physicsScene->CheckCollisionsOnPoint(worldPos, false, true);

		// Reset last object
		if (selectedObject != nullptr) {
			selectedObject->colour -= 0.75;
		}

		if (col != nullptr && selectedObject != col) {
			std::cout << "Collide" << std::endl;
			col->colour += 0.75;
		}
		selectedObject = col;
	}

	if (selectedObject != nullptr && (input->wasKeyPressed(aie::INPUT_KEY_DELETE) || input->wasKeyPressed(aie::INPUT_KEY_BACKSPACE))) {
		physicsScene->RemoveActor(selectedObject);
		selectedObject = nullptr;
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_X)) {
		Rope(ropeSize, worldPos, true);
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_Z)) {
		Rope(ropeSize, worldPos, false);
	}

	shouldErase = input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT);

	bool isKinematic = input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL);

	// Circle drag
	if (input->isKeyDown(aie::INPUT_KEY_C) && input->isMouseButtonDown(0)) {

		if (!circleDrag) {
			dragStart = worldPos;
			circleDrag = true;
		}
		dragEnd = worldPos;
	}
	// If key is no longer pressed but mouse is still pressed, cancel action
	else if (circleDrag && input->isMouseButtonDown(0)) {
		circleDrag = false;
	}
	// If drag stops
	else if (circleDrag) {
		circleDrag = false;

		float radius = glm::length(dragEnd - dragStart) * 0.5f;
		glm::vec2 center = (dragEnd + dragStart) * 0.5f;

		Circle* circle = new Circle(center, glm::vec2(0, 0), 10.0f, fmax(0.1f, radius), 0.5f, shouldErase ? glm::vec4(0) : glm::vec4(0, 1, 0, 1));
		if (shouldErase) {
			circle->eraser = shouldErase;
			// Adding some angular velocity fixes some collision detection problems for some reason
			circle->angularVelocity = 2.0f;
		}
		circle->isKinematic = isKinematic;
		physicsScene->AddActor(circle);
	}

	// Square drag
	if (input->isKeyDown(aie::INPUT_KEY_S) && input->isMouseButtonDown(0)) {

		if (!boxDrag) {
			dragStart = worldPos;
			boxDrag = true;
		}
		dragEnd = worldPos;
	}
	// If key is no longer pressed but mouse is still pressed, cancel action
	else if (boxDrag && input->isMouseButtonDown(0)) {
		boxDrag = false;
	}
	// If drag stops
	else if (boxDrag) {
		boxDrag = false;

		glm::vec2 diff = (dragEnd - dragStart) * 0.5f;
		glm::vec2 center = (dragEnd + dragStart) * 0.5f;

		Box* box = new Box(center, glm::vec2(fmax(0.1f, abs(diff.x)), fmax(0.1f, abs(diff.y))), glm::vec2(0, 0), 90.0f, 0.5f, shouldErase ? glm::vec4(0) : glm::vec4(1, 0, 0, 1));
		if (shouldErase) {
			box->eraser = shouldErase;
			// Adding some angular velocity fixes some collision detection problems for some reason
			box->angularVelocity = 2.0f;
		}
		box->isKinematic = isKinematic;
		physicsScene->AddActor(box);
	}

	// Plane drag
	if (input->isKeyDown(aie::INPUT_KEY_Q) && input->isMouseButtonDown(0)) {

		if (!planeDrag) {
			dragStart = worldPos;
			planeDrag = true;
		}
		dragEnd = worldPos;
	}
	// If key is no longer pressed but mouse is still pressed, cancel action
	else if (planeDrag && input->isMouseButtonDown(0)) {
		planeDrag = false;
	}
	// If drag stops
	else if (planeDrag) {
		planeDrag = false;

		glm::vec2 diff = (dragEnd - dragStart);
		glm::vec2 center = (dragEnd + dragStart) * 0.5f;
		glm::vec2 toOrigin = glm::vec2(0, 0) - center;
		float dir = (planeClockwise ? 1.f : -1.f);
		glm::vec2 normal = dir * glm::normalize(glm::vec2(diff.y, -diff.x));

		float dist = -glm::dot(toOrigin, normal);

		Plane* plane = new Plane(normal, dist, 0.5f, glm::vec4(0.5f, 0, 1, 1));
		//box->eraser = shouldErase;
		physicsScene->AddActor(plane);
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_TAB)) {
		planeClockwise = !planeClockwise;
	}



	if (input->isMouseButtonDown(2)) {
		if (!middleMouseDrag) {
			middleMouseDragStart = worldPos;
			middleMouseDrag = true;
		}

		middleMouseDragDist = (worldPos - middleMouseDragStart);
	}
	else if (middleMouseDrag) {
		middleMouseDrag = false;
		orthoCenter -= middleMouseDragDist;
		middleMouseDragDist = glm::vec2(0);
	}

	double dir = input->getMouseScroll();
	if (dir != lastScrollValue) {
		double deltaDir = (dir - lastScrollValue) * (input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL) ? 1 : (orthoSize / 1000));
		lastScrollValue = dir;
		orthoSize = fmin(maxOrthoSize, fmax(1, orthoSize - deltaDir * configValues["SCROLL_SENSITIVITY"]));
	}
}



void PhysicsEngine::draw() {

	// begin drawing sprites
	m_2dRenderer->begin();

	if (configSettings["TEST_MODE"] == 1) {
		if (circleDrag) {
			float radius = glm::length(dragEnd - dragStart) * 0.5f;
			glm::vec2 center = (dragEnd + dragStart) * 0.5f;

			aie::Gizmos::add2DCircle(center, radius, 12, shouldErase ? glm::vec4(1, 0, 0, 0.5f) : glm::vec4(1, 1, 1, 0.5f));

			char info[100];
			sprintf_s(info, 100, "%sCircle: Pos: (%i,%i), Radius: %i", shouldErase ? "Eraser " : "", (int)center.x, (int)center.y, (int)radius);
			m_2dRenderer->drawText(m_font, info, 5, 10);
		}
		else if (boxDrag) {
			glm::vec2 center = (dragEnd + dragStart) * 0.5f;
			glm::vec2 diff = (dragEnd - dragStart) * 0.5f;
			glm::vec2 absDiff = glm::vec2(abs(diff.x), abs(diff.y));
			aie::Gizmos::add2DAABBFilled(center, absDiff, shouldErase ? glm::vec4(1, 0, 0, 0.5f) : glm::vec4(1, 1, 1, 0.5f));

			char info[100];
			sprintf_s(info, 100, "%sBox: Pos: (%i,%i), Extents: (%i, %i)", shouldErase ? "Eraser " : "", (int)center.x, (int)center.y, (int)absDiff.x, (int)absDiff.y);
			m_2dRenderer->drawText(m_font, info, 5, 10);
		}
		else if (planeDrag) {
			glm::vec2 diff = (dragEnd - dragStart);
			glm::vec2 center = (dragEnd + dragStart) * 0.5f;
			glm::vec2 toOrigin = glm::vec2(0, 0) - center;
			float dir = (planeClockwise ? 1.f : -1.f);
			glm::vec2 normal = dir * glm::normalize(glm::vec2(diff.y, -diff.x));

			float dist = -glm::dot(toOrigin, normal);

			// Draw plane
			aie::Gizmos::add2DLine(dragStart, dragEnd, glm::vec4(1));
			// Draw distance from center
			aie::Gizmos::add2DLine(glm::vec2(0), normal * dist, glm::vec4(1, 0.5f, 0, 1));
			// Draw normal on plane
			aie::Gizmos::add2DLine(center, center + normal * orthoSize * 0.02f, glm::vec4(1, 0, 0, 1));

			char info[100];
			sprintf_s(info, 100, "Plane: Normal: (%.2f,%.2f), Dist: %i | [Tab] Reverse normal", normal.x, normal.y, (int)dist);
			m_2dRenderer->drawText(m_font, info, 5, 10);
		}
		else if (selectedObject != nullptr) {


			std::string shapeType = selectedObject->GetShapeID() == BOX ? "Box" : selectedObject->GetShapeID() == CIRCLE ? "Circle" : "Plane";
			char info[100];
			sprintf_s(info, 100, "Selected Object: %s", shapeType.c_str());
			m_2dRenderer->drawText(m_font, info, 5, 10);
		}
		else {
			m_2dRenderer->drawText(m_font, "R to refresh config", 5, 10);
		}
	}
	

	// wipe the screen to the background colour
	clearScreen();


	//DrawText("Hello", glm::vec2(-100, 0));
	// draw your stuff here!
	aspectRatio = configSettings["AUTO_CALCULATE_ASPECT_RATIO"] == 1 ? 
		(configValues["APPLICATION_WINDOW_WIDTH"] / configValues["APPLICATION_WINDOW_HEIGHT"]) :
		(configValues["APPLICATION_ASPECT_RATIO_NUMERATOR"] / configValues["APPLICATION_ASPECT_RATIO_DENOMINATOR"]);
	glm::vec2 tempOrthoCenter = middleMouseDrag ? (orthoCenter - middleMouseDragDist) : orthoCenter;
	aie::Gizmos::draw2D(glm::ortho<float>(-orthoSize+ tempOrthoCenter.x, orthoSize+ tempOrthoCenter.x, -orthoSize / aspectRatio + tempOrthoCenter.y, orthoSize / aspectRatio + tempOrthoCenter.y, -1.0f, 1.0f));


	// output some text, uses the last used colour
	//m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);
	//DrawText("Hello", glm::vec2(-100, 0));
	// done drawing sprites

	if (configSettings["SHOW_FPS"] == 1) {
		char fps[32];
		sprintf_s(fps, 32, "FPS: %i", getFPS());
		m_2dRenderer->drawText(m_font, fps, 0, getWindowHeight() - 32);
	}

	if (configSettings["TEST_MODE"] == 1) {
		char zoom[32];
		sprintf_s(zoom, 32, "Zoom: %.2fx", round((orthoSize / 1000) * 100) / 100);
		m_2dRenderer->drawText(m_font, zoom, 5, 40);

		m_2dRenderer->end();
	}



}


glm::vec2 PhysicsEngine::GetScreenSpacePoint(float xWorld, float yWorld) {
	int x = ((xWorld + orthoSize) / (orthoSize * 2.f)) * getWindowWidth();
	int y = ((yWorld + orthoSize / aspectRatio) / (orthoSize / aspectRatio * 2.f)) * getWindowHeight();

	return glm::vec2(x, y);
}

glm::vec2 PhysicsEngine::GetWorldSpacePoint(float xScreen, float yScreen) {
	// AIE bootstrap uses the ortho value ranges for drawing things on the screen. So convert screen point to ortho point
	float x = (((xScreen / getWindowWidth()) * orthoSize * 2) - orthoSize) + orthoCenter.x;
	float y = ((((yScreen / getWindowHeight()) * orthoSize * 2) - orthoSize) / aspectRatio) + orthoCenter.y;
	return glm::vec2(x, y);
}

void PhysicsEngine::LoadConfig() {
	configSettings.clear();
	configValues.clear();

	std::ifstream is_file("config.txt");
	std::fstream is_file_build("buildConfig.txt", std::ifstream::in);
	bool isBuild = false;


	// If program is not a build (has a regular config file and not a release one)
	if (is_file && !is_file_build) {
		std::cout << "Loading Config..." << std::endl;
		std::string line;
		// Get every line
		while (std::getline(is_file, line))
		{
			std::istringstream iss(line);
			std::string key;
			// Get the key name
			if (std::getline(iss, key, '='))
			{
				std::string value;
				// Get the value
				if (std::getline(iss, value))
				{
					// Value setting
					if (key.find("V_") != std::string::npos) {
						// Remove prefix from key
						key.erase(0, 2);
						float v = std::stof(value);
				
						configValues.emplace(key, v);
						std::cout << "Value: " << key << " = " << value << std::endl;

					}
					// Int setting
					else {

						int i = std::stoi(value);

						// If this config needs to be encrypted
						if (key == "ENCRYPT" && i == 1) {
							isBuild = true;
						}
						configSettings.emplace(key, i);
						std::cout << "Setting: " << key << " = " << value << std::endl;
					}
				}
			}
		}
		// Close the file
		is_file.close();
	}
	// If this is a release build
	else if (is_file_build) {
		is_file.close();
		// Remove old config file
		if (is_file_build) {
			std::remove("config.txt");
		}

		std::string line;
		// Get every line
		while (std::getline(is_file_build, line))
		{
			// Decrypt the line
			std::istringstream iss(Decrypt(line, "293supersecure342encryption994key221LOL"));
			std::string key;
			// Get the key name
			if (std::getline(iss, key, '='))
			{
				std::string value;
				// Get the value
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

						// Don't need to store whether this is encrypted
						if (key == "ENCRYPT") {
							continue;
						}

						int i = std::stoi(value);
						configSettings.emplace(key, i);

					}
				}
			}
		}
	}
	
	
	// Encrypt the config file (used for release builds where config shouldn't be changed)
	if(isBuild) {
		// Reoppen original config
		std::ifstream is_file_original("config.txt");
		// If build file hasn't been created yet
		if (is_file_original && !is_file_build) {
			is_file_build.open("buildConfig.txt", std::ifstream::out);
			std::string line;
			// Encrypt all data from config file, and put it into new build config
			while (std::getline(is_file_original, line))
			{
				if (line.find("#") == std::string::npos && line != "")
				{
					std::string encrypted = Encrypt(line, "293supersecure342encryption994key221LOL");
					char* char_array = new char[encrypted.length() + 1];
					strcpy(char_array, encrypted.c_str());

					is_file_build << encrypted << "\n";
				}

			}
		}
		is_file_original.close();
		std::remove("config.txt");
	}
	is_file_build.close();
}

std::string PhysicsEngine::Encrypt(std::string text, std::string key) {
	std::string temp = key;
	while (key.length() < text.length())
		key += temp;

	for (int i = 0; i < text.length(); i++) {
		text[i] ^= key[i];
	}
	return text;
}

std::string PhysicsEngine::Decrypt(std::string text, std::string key) {
	return Encrypt(text, key);
}


float PhysicsEngine::CalculateCoefficientRestitution(float restitution1, float restitution2) {

	return CoefficientCalculationFunctions[configSettings["RESTITUTION_CALC"]](restitution1, restitution2);
}

float PhysicsEngine::CalculateCoefficientStaticFriction(float restitution1, float restitution2) {

	return CoefficientCalculationFunctions[configSettings["STATFRICTION_CALC"]](restitution1, restitution2);
}

