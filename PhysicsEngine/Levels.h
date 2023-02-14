#pragma once
#include "PhysicsScene.h"
#include <string>

class Levels
{
public:
	Levels() {};
	~Levels() {};

	void Level1(PhysicsScene* scene);

	static void SaveToConsole(std::vector<PhysicsObject*> actors);
};
