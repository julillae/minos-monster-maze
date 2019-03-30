#pragma once

#include "gameState.hpp"
#include "../menus/pauseMenu.hpp"
#include "../level.hpp"
#include "../menus/button.hpp"
#include "../menus/mainButton.hpp"
#include "../menus/savePopup.hpp"
#include "../gameSave.hpp"
#include "mainMenuState.hpp"

extern Level* world;

class LevelSelectState;

class PauseMenuState : public GameState
{
public:
    PauseMenuState(Game* game);

    void init(vec2 screen);

    void draw()override;
    // Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms)override ;
    void on_key(GLFWwindow*, int key, int, int action, int mod)override;
    bool is_over()override;
    void destroy()override;
    void reset_buttons();

private:
    void init_buttons();
    void set_currentButton(MainButton* button);

    PauseMenu pauseMenu;
    SavePopup savePopup;

    MainButton* currentButton;
    int buttonIndex = 0;

    // menu buttons
    MainButton* pauseButtons[5];

    MainButton continueButton;
    MainButton restartButton;
    MainButton saveButton;
    MainButton controlsButton;
    MainButton mainMenuButton;
    bool show_help_menu = false;

    HelpMenu m_help_menu;

    vec2 initialPosition;
};