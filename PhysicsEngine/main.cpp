#include "PhysicsEngine.h"

int main() {
	
	// allocation
	auto app = new PhysicsEngine();

	// initialise and loop
	app->run("AIE", app->configValues["APPLICATION_WINDOW_WIDTH"], app->configValues["APPLICATION_WINDOW_HEIGHT"], app->configSettings["APPLICATION_WINDOW_FULLSCREEN"] == 1);
	// deallocation
	delete app;

	return 0;
}