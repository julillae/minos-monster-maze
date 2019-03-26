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

    vec2 screen = { (float)width, (float)height };
    game.init(screen);

	mmState->init(screen);
	game.push_state(mmState);
	game.set_current_state(mmState);
	game.game_loop();

	return EXIT_SUCCESS;
}