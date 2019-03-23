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
    std::vector<GameState*> get_states();
    void set_current_state(GameState* currState);

    void game_loop();

    // Window handle
    GLFWwindow* m_window;

private:

    GLuint m_frame_buffer;

    std::vector<GameState*> states; //TODO: use a map and create enum for different states

    GameState* currentState;

};