#pragma once

#include "gameState.hpp"
#include "../level.hpp"
#include "../gameSave.hpp"
#include "../menus/minotaurIntro.hpp"

extern Level* world;

class MinotaurIntroState : public GameState
{
public:
    MinotaurIntroState(Game* game);

    void init(vec2 screen);

    void draw()override;
    // Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms)override ;
    void on_key(GLFWwindow*, int key, int, int action, int mod)override;
    bool is_over()override;
    void destroy()override;

    bool should_show();
    void load_minotaur_level();

private:
    MinotaurIntro minotaur_intro;

    float m_start_time;
    float minotaur_intro_length = 18.f;
    int minotaur_level = 12;
    
    bool show_help_menu = false;

    HelpMenu m_help_menu;

    Mix_Music* m_background_music;
    vec2 initialPosition;

    vec2 currPosition;
};