#pragma once

#include "gameState.hpp"
#include "../menus/levelSelectMenu.hpp"
#include "../level.hpp"
#include "../menus/button.hpp"
#include "../menus/levelButton.hpp"
#include "mainMenuState.hpp"

class Level;

class LevelSelectState : public GameState
{
public:
    LevelSelectState(Game* game);

    void init(vec2 screen);

    void draw()override;
    // Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms)override ;
    void on_key(GLFWwindow*, int key, int, int action, int mod)override;
    bool is_over()override;
    void destroy()override;

    void init_buttons(float osScaleFactor);
    void set_currentButton(LevelButton* button);
    void reset_buttons();
private:
    LevelSelectMenu levelSelectMenu;
    LevelButton* currentButton;

    // menu buttons
    LevelButton* levelButtons[12];

    LevelButton level1Button;
    LevelButton level2Button;
    LevelButton level3Button;
    LevelButton level4Button;
    LevelButton level5Button;
    LevelButton level6Button;
    LevelButton level7Button;
    LevelButton level8Button;
    LevelButton level9Button;
    LevelButton level10Button;
    LevelButton level11Button;
    LevelButton level12Button;

    vec2 initialPosition;

    bool close = false;

};