#include "gameState.hpp"
#include "../menus/mainMenu.hpp"
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

private:
    MainMenu mainMenu;
    Mix_Music* m_background_music;

};