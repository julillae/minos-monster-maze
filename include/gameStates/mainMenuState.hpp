#pragma once

#include "gameState.hpp"
#include "../menus/mainMenu.hpp"
#include "../menus/continueButton.hpp"
#include "../menus/newGameButton.hpp"
#include "../menus/controlsButton.hpp"
#include "../menus/quitButton.hpp"
#include "../level.hpp"

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

    void initialize_camera_position(int w, int h);

    void init_buttons();
    void set_currentButton(Menu* button);

private:
    MainMenu mainMenu;
    Menu* currentButton;

    // menu buttons
    ContinueButton continueButton;
    NewGameButton newGameButton;
    ControlsButton controlsButton;
    QuitButton quitButton;
    bool show_help_menu = false;

    HelpMenu m_help_menu;

    Mix_Music* m_background_music;
    vec2 initialPosition;

    bool close = false;

};