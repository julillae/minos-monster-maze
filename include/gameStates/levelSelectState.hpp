#pragma once

#include "gameState.hpp"
#include "../menus/levelSelectMenu.hpp"
#include "../level.hpp"
#include "../menus/button.hpp"
#include "../menus/levelButton.hpp"

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

    void initialize_camera_position(int w, int h);

    void init_buttons();
    void set_currentButton(LevelButton* button);

private:
    LevelSelectMenu levelSelectMenu;
    LevelButton* currentButton;

    // menu buttons
    LevelButton* levelButtons[10];

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

    vec2 initialPosition;

    bool close = false;

};