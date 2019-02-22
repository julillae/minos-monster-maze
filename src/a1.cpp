// internal
#include "../include/common.hpp"
#include "../include/physics.hpp"
#include "../include/level.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>
#include <math.h>

using Clock = std::chrono::high_resolution_clock;

// Global 
Level world;
const int width = 1280;
const int height = 720;
const char* title = "Your Title Here";

// Entry point
int main(int argc, char* argv[])
{
	// Initializing world (after renderer.init().. sorry)
	Physics *physicsHandler = new Physics();
	std::string levelName = "level0";
	if (!world.init({ (float)width, (float)height }, physicsHandler, levelName))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();
	float dt = 1000.0 / 60.0;	// set for 60fps display
	float timeAcc = 0.f;

	// fixed update timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		timeAcc += elapsed_sec;

		while (timeAcc >= dt) {
			world.update(timeAcc/dt);
			timeAcc -= dt;
		}
		world.draw();
	}

	world.destroy();

	return EXIT_SUCCESS;
}