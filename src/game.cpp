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
    while(!states.empty())
        pop_state();
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

    //-------------------------------------------------------------------------
    // Loading music and sounds
//    if (SDL_Init(SDL_INIT_AUDIO) < 0)
//    {
//        fprintf(stderr, "Failed to initialize SDL Audio");
//        return false;
//    }
//
//    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
//    {
//        fprintf(stderr, "Failed to open audio device");
//        return false;
//    }
//
//    //Note: the following music credits needs to be added to a credit scene at the end of the game
//    //Secret Catacombs
//    //by Eric Matyas
//    //www.soundimage.org
//
//    m_background_music = Mix_LoadMUS(audio_path("secret_catacombs.wav"));
//
//    if (m_background_music == nullptr)
//    {
//        fprintf(stderr, "Failed to load sound\n %s,%s\n make sure the data directory is present",
//                audio_path("salmon_dead.wav"),
//                audio_path("secret_catacombs.wav"));
//        return false;
//    }
//
//    // Playing background music undefinitely
//    Mix_PlayMusic(m_background_music, -1);
//    Mix_VolumeMusic(50);
//
//    fprintf(stderr, "Loaded music\n");
    return true;
}

void Game::push_state(GameState* state)
{
    states.push_back(state);
}

void Game::pop_state()
{
    states.back()->destroy();
    //delete states.back();
    states.pop_back();
}

GameState* Game::current_state()
{
    if (states.empty())
        return nullptr;
    else
        return states.back();
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
		}
		current_state()->draw();
	}
}
