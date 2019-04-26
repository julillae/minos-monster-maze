#include "../include/game.hpp"
#include "../include/gameStates/gameState.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>
#include <math.h>

namespace
{
    void glfw_err_cb(int error, const char* desc)
    {
        fprintf(stderr, "%d: %s", error, desc);
    }
}

using Clock = std::chrono::high_resolution_clock;

Game::Game()
{

}

Game::~Game()
{
    for (auto const& state : states)
    {
        state.second->destroy();
    }
    glfwTerminate();
}

bool Game::init(vec2 screen)
{
    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization
    // Core Opengl 3.
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);
    m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "A1 Assignment", nullptr, nullptr);
    if (m_window == nullptr)
        return false;

    // hack used to make sure the display for macOS with retina display issue is consistent with display on other systems
    int testWidth;
    glfwGetFramebufferSize(const_cast<GLFWwindow *>(m_window), &testWidth, nullptr);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    // Load OpenGL function pointers
    gl3w_init();

    // Create a frame buffer
    m_frame_buffer = 0;
    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
	soundManager.init();
    return true;
}

void Game::push_state(GameState* state)
{
    states.insert( std::pair<gameStates, GameState*>(state->name, state));
}

GameState* Game::get_state(gameStates stateName)
{
    if (states.find(stateName) != states.end())
        return states.find(stateName)->second;
    else
        return NULL;
}

GameState* Game::current_state()
{
    if (states.empty())
        return nullptr;
    else
        return currentState;
}

void Game::game_loop()
{
    auto t = Clock::now();
	float dt = 1000.0 / 60.0;	// set for 60fps display
	float timeAcc = 0.f;

	// fixed update timestep loop.. can be improved (:
	while (!current_state()->is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		timeAcc += elapsed_sec;

		while (timeAcc >= dt) {
			current_state()->update(timeAcc/dt);
			timeAcc -= dt;
			// Hotfix for loading large levels and from saved game
			if (timeAcc > 100.f)
			    timeAcc = 100.f;
		}
		current_state()->draw();
	}
}

std::map<gameStates, GameState*> Game::get_states()
{
    return states;
}

void Game::set_current_state(GameState* currState)
{
	if (currentState == NULL) {
		soundManager.play_menu_bg_music();
	} else if (currentState->name == LEVEL || currentState->name == PAUSE) {
		if (currState->name == MAIN || currState->name == LEVELSELECT || currState->name == CREDITS)
			soundManager.play_menu_bg_music();
	}
	else {
		if ((currentState->name == INTRO || currentState->name == MINOTAURINTRO) && currState->name == LEVEL) {
			// do nothing, should already be playing level_bg_music
		}
		else if (currState->name == LEVEL || currState->name == PAUSE || currState->name == INTRO || currState->name == MINOTAURINTRO) {
			soundManager.play_level_bg_music();
		}
	}
    currentState = currState;
    currentState->set_onKey();
}
