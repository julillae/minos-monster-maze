#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>
#include <map>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

class GameState;

// Game class
class Game
{
public:

    Game();
    ~Game();

    bool init(vec2 screen);

    void push_state(GameState* state);
    void pop_state();

    GameState* current_state();

    void game_loop();

    // Window handle
    GLFWwindow* m_window;

private:

    // Part of hack needed to deal with the MacOs Retina Display issue where it doubles the pixels rendered
    float osScaleFactor = 1.f;

    // Screen texture
    // The draw loop first renders to this texture, then it is used for the water shader
    GLuint m_frame_buffer;

    Mix_Music* m_background_music;

    std::vector<GameState*> states;

};