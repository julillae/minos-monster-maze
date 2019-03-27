#include "../../include/gameStates/levelSelectState.hpp"
#include "../../include/renderManager.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>

Level* world;

namespace
{
    vec2 cameraCenter;
}

LevelSelectState::LevelSelectState(Game *game)
{
    this->game = game;

}

void LevelSelectState::init(vec2 screen)
{
    fprintf(stderr, "level select init\n");
    this->m_window = game->m_window;
    name = LEVELSELECT;

    // hack used to make sure the display for macOS with retina display issue is consistent with display on other systems
    int testWidth;
    glfwGetFramebufferSize(m_window, &testWidth, nullptr);
    osScaleFactor = testWidth / screen.x;

    m_position = vec2({screen.x / 2, screen.y / 2});

    m_screen = screen;

    // Load OpenGL function pointers
    gl3w_init();

    set_onKey();

    // Create a frame buffer
    m_frame_buffer = 0;
    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    glViewport(0, 0, w, h);

    initialPosition = vec2({static_cast<float>(w / 2), static_cast<float>(h / 2)});

    levelSelectMenu.init(initialPosition);
    init_buttons();
    initialize_camera_position(w, h);

}

void LevelSelectState::draw()
{
    // Clearing error buffer
    gl_flush_errors();

    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    // Updating window title with points
    std::stringstream title_ss;
    title_ss << "Minos' Monster Maze";
    glfwSetWindowTitle(m_window, title_ss.str().c_str());

    /////////////////////////////////////
    // First render to the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
    const float clear_color[3] = { 0.5f, 0.5f, 0.5f };

    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w;// *0.5;
    float bottom = (float)h;// *0.5;

    float sx = 2.f * osScaleFactor / (right - left);
    float sy = 2.f * osScaleFactor / (top - bottom); //this is where you play around with the camera

    // initialize camera position
    int camera_w, camera_h;
    glfwGetWindowSize(m_window, &camera_w, &camera_h);

    float txOffset = camera_w / 2;
    float tyOffset = camera_h / 2;
    cameraCenter = vec2({ txOffset, tyOffset});

    levelSelectMenu.set_position(cameraCenter);

    tx = -cameraCenter.x;
    ty = -cameraCenter.y;

    mat3 scaling_matrix = { {sx, 0.f, 0.f },
                            { 0.f, sy, 0.f },
                            { 0.f, 0.f, 1.f } };


    mat3 translation_matrix = { {1.f, 0.f, 0.f},
                                {0.f, 1.f, 0.f},
                                {tx, ty, 1.f} };

    mat3 projection_2D{ { 1.f, 0.f, 0.f },
                        { 0.f, 1.f, 0.f },
                        { 0.f, 0.f, 1.f } };

    projection_2D = mul(projection_2D, scaling_matrix);
    projection_2D = mul(projection_2D, translation_matrix);

    /////////////////////
    // Truely render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0, 10);
    glClearColor(0, 0, 0, 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);

    //////////////////

    levelSelectMenu.draw(projection_2D);

    for (auto& button : levelButtons){
        button->draw(projection_2D);
    }

    // Presenting
    glfwSwapBuffers(m_window);
}

bool LevelSelectState::update(float elapsed_ms)
{
    return true;
}

void LevelSelectState::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ENTER)
        {
            Level* level = (Level*) game->get_state(LEVEL);
            if (level != NULL)
            {
                level->load_select_level(currentButton->level);
                game->set_current_state(level);
            } else
            {
                Physics *physicsHandler = new Physics();
                Level* newLevel = new Level(game);
                newLevel->init(m_screen, physicsHandler, currentButton->level);
                game->push_state(newLevel);
                game->set_current_state(newLevel);
            }

            world = (Level*) game->get_state(LEVEL);

        }

        int numButtons = sizeof(levelButtons)/sizeof(*levelButtons);

        if (key == GLFW_KEY_RIGHT)
        {

            int nextButton = (currentButton->level + 1) % numButtons;
            set_currentButton(levelButtons[nextButton]);
        }

        if (key == GLFW_KEY_LEFT)
        {

            int nextButton = currentButton->level - 1;
            if (nextButton < 0)
                nextButton = numButtons - 1;
            set_currentButton(levelButtons[nextButton]);
        }

        if (key == GLFW_KEY_DOWN && currentButton->level < 4)
        {
            int nextButton = (currentButton->level + 6) % numButtons;
            set_currentButton(levelButtons[nextButton]);
        }

        if (key == GLFW_KEY_UP && currentButton->level >=5)
        {
            int nextButton = (currentButton->level - 6);
            set_currentButton(levelButtons[nextButton]);
        }


        if (key == GLFW_KEY_ESCAPE)
        {
            GameState* mainMenuState = game->get_states().find(MAIN)->second;
            game->set_current_state(mainMenuState);
        }

    }
}

bool LevelSelectState::is_over()
{
    return glfwWindowShouldClose(m_window) || close;
}

void LevelSelectState::destroy()
{
    glDeleteFramebuffers(1, &m_frame_buffer);

    levelSelectMenu.destroy();
    for (auto& button : levelButtons) {
        button->destroy();
    }

}

void LevelSelectState::initialize_camera_position(int w, int h)
{
        float txOffset = w / 2;
        float tyOffset = h / 2;
        cameraCenter = vec2({ txOffset, tyOffset});

}

void LevelSelectState::init_buttons()
{
    float buttonX = initialPosition.x / 3 + 50;
    float buttonY = initialPosition.y - 85;
    float buttonOffset_x = 150.f;
    float buttonOffset_y = 200.f;


    const char* level1Text = textures_path("level1.png");
    const char* level2Text = textures_path("level2.png");
    const char* level3Text = textures_path("level3.png");
    const char* level4Text = textures_path("level4.png");
    const char* level5Text = textures_path("level5.png");
    const char* level6Text = textures_path("level6.png");
    const char* level7Text = textures_path("level7.png");
    const char* level8Text = textures_path("level8.png");
    const char* level9Text = textures_path("level9.png");
    const char* level10Text = textures_path("level10.png");

    level1Button.init(vec2({buttonX, buttonY}), level1Text, 0 );
    level1Button.set_selected(true);
    currentButton = &level1Button;
    level2Button.init(vec2({buttonX + buttonOffset_x, buttonY}), level2Text, 1);
    level3Button.init(vec2({buttonX + buttonOffset_x * 2, buttonY }), level3Text, 2);
    level4Button.init(vec2({buttonX + buttonOffset_x * 3, buttonY }), level4Text, 3);
    level5Button.init(vec2({buttonX + buttonOffset_x * 4, buttonY }), level5Text, 4);
    level6Button.init(vec2({buttonX + buttonOffset_x * 5, buttonY }), level6Text, 5);

    buttonY = buttonY + buttonOffset_y;

    level7Button.init(vec2({buttonX + buttonOffset_x * 0, buttonY }), level7Text, 6);
    level8Button.init(vec2({buttonX + buttonOffset_x * 1, buttonY }), level8Text, 7);
    level9Button.init(vec2({buttonX + buttonOffset_x * 2, buttonY }), level9Text, 8);
    level10Button.init(vec2({buttonX + buttonOffset_x * 3, buttonY }), level10Text, 9);

    levelButtons[0] = &level1Button;
    levelButtons[1] = &level2Button;
    levelButtons[2] = &level3Button;
    levelButtons[3] = &level4Button;
    levelButtons[4] = &level5Button;
    levelButtons[5] = &level6Button;
    levelButtons[6] = &level7Button;
    levelButtons[7] = &level8Button;
    levelButtons[8] = &level9Button;
    levelButtons[9] = &level10Button;

}

void LevelSelectState::set_currentButton(LevelButton* button)
{
    currentButton->set_selected(false);
    button->set_selected(true);
    currentButton = button;

}

void LevelSelectState::reset_buttons()
{
    set_currentButton(levelButtons[0]);
}
