// internal
#include "../include/common.hpp"
#include "../include/physics.hpp"
#include "../include/level.hpp"
#include "../include/a1.hpp"
#include "../include/game.hpp"
#include "../include/gameStates/mainMenuState.hpp"

#define GL3W_IMPLEMENTATION
//#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>
#include <math.h>

using Clock = std::chrono::high_resolution_clock;

// Global
Game game;
Level *world = new Level(&game);
MainMenuState *mmState = new MainMenuState(&game);
const int width = 1280;
const int height = 720;

// Entry point
int main(int argc, char* argv[])
{
	// Initializing world (after renderer.init().. sorry)
	Physics *physicsHandler = new Physics();
	int startLevel = 0;

    vec2 screen = { (float)width, (float)height };
    game.init(screen);

//	if (!world.init({ (float)width, (float)height }, physicsHandler, startLevel))
//	{
//		// Time to read the error message
//		std::cout << "Press any key to exit" << std::endl;
//		std::cin.get();
//		return EXIT_FAILURE;
//	}
//
//	auto t = Clock::now();
//	float dt = 1000.0 / 60.0;	// set for 60fps display
//	float timeAcc = 0.f;
//
//	// fixed update timestep loop.. can be improved (:
//	while (!world.is_over())
//	{
//		// Processes system messages, if this wasn't present the window would become unresponsive
//		glfwPollEvents();
//
//		// Calculating elapsed times in milliseconds from the previous iteration
//		auto now = Clock::now();
//		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
//		t = now;
//		timeAcc += elapsed_sec;
//
//		while (timeAcc >= dt) {
//			world.update(timeAcc/dt);
//			timeAcc -= dt;
//		}
//		world.draw();
//	}
//
//	world.destroy();

	auto t = Clock::now();
	float dt = 1000.0 / 60.0;	// set for 60fps display
	float timeAcc = 0.f;

//    mmState->init(screen);
//	// fixed update timestep loop.. can be improved (:
//	while (!mmState->is_over())
//	{
//		// Processes system messages, if this wasn't present the window would become unresponsive
//		glfwPollEvents();
//
//		// Calculating elapsed times in milliseconds from the previous iteration
//		auto now = Clock::now();
//		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
//		t = now;
//		timeAcc += elapsed_sec;
//
//		while (timeAcc >= dt) {
//            mmState->update(timeAcc/dt);
//			timeAcc -= dt;
//		}
//        mmState->draw();
//	}
//
//    mmState->destroy();

//    world->init(screen, physicsHandler, startLevel);
//    // fixed update timestep loop.. can be improved (:
//    while (!world->is_over())
//    {
//        // Processes system messages, if this wasn't present the window would become unresponsive
//        glfwPollEvents();
//
//        // Calculating elapsed times in milliseconds from the previous iteration
//        auto now = Clock::now();
//        float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
//        t = now;
//        timeAcc += elapsed_sec;
//
//        while (timeAcc >= dt) {
//            world->update(timeAcc/dt);
//            timeAcc -= dt;
//        }
//        world->draw();
//    }
//
//    world->destroy();


//	Level* level = new Level(&game);
//	level->init(screen, physicsHandler, startLevel);
//	game.push_state(level);
	//MainMenuState* mainMenuState = new MainMenuState(&game);
	mmState->init(screen);
	game.push_state(mmState);
	game.set_current_state(mmState);
	game.game_loop();

	return EXIT_SUCCESS;
}