#pragma once

#include "gameState.hpp"
#include "../menus/mainMenu.hpp"
#include "../level.hpp"
#include "../menus/button.hpp"
#include "../menus/mainButton.hpp"
#include "levelSelectState.hpp"

class MainMenuState : public GameState
{
public:
    MainMenuState(Game* game);

    void init(vec2 screen);

    void draw()override;
    // Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms)override ;
    void on_key(GLFWwindow*, int key, int, int action, int mod)override;
    bool is_over()override;
    void destroy()override;

    void init_buttons();
    void set_currentButton(MainButton* button);
    void reset_buttons();

private:
    MainMenu mainMenu;
    MainButton* currentButton;

    // menu buttons
    MainButton continueButton;
    MainButton newGameButton;
    MainButton controlsButton;
    MainButton quitButton;
    bool show_help_menu = false;
    bool show_continue = false;

    HelpMenu m_help_menu;

    Mix_Music* m_background_music;
    vec2 initialPosition;

    bool close = false;

};