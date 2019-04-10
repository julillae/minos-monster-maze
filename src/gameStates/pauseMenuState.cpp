#include "../../include/gameStates/pauseMenuState.hpp"
#include "../../include/renderManager.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>

PauseMenuState::PauseMenuState(Game *game)
{
    this->game = game;
	soundManager = &game->soundManager;
}

void PauseMenuState::init(vec2 screen)
{
    fprintf(stderr, "pause menu init\n");
    this->m_window = game->m_window;
    name = PAUSE;

    // hack used to make sure the display for macOS with retina display issue is consistent with display on other systems
    int testWidth;
    glfwGetFramebufferSize(m_window, &testWidth, nullptr);
    osScaleFactor = testWidth / screen.x;

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

    pauseMenu.init(initialPosition);
    m_help_menu.init(initialPosition);
    m_help_menu.set_visibility(false);
    init_buttons();
    savePopup.init(initialPosition);
    savePopup.set_visibility(false);
    initialize_camera_position(w, h);
    pauseMenu.set_position(cameraCenter);

}

void PauseMenuState::draw()
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

    render_to_framebuffer_screen();

    mat3 projection_2D = calculate_projection();

    pauseMenu.draw(projection_2D);

    for (auto& button : pauseButtons) {
        button->draw(projection_2D);
    }

    m_help_menu.draw(projection_2D);
    savePopup.draw(projection_2D);
    // Presenting
    glfwSwapBuffers(m_window);
}

bool PauseMenuState::update(float elapsed_ms)
{
    float popupDuration = 2.f;
    if (savePopup.get_time_since_popup() > popupDuration)
        savePopup.set_visibility(false);

    return true;
}

void PauseMenuState::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    int numButtons = sizeof(pauseButtons)/sizeof(*pauseButtons);

    if (action == GLFW_PRESS) {
        if (!show_help_menu)
        {
			if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT ||
				key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)
				soundManager->play_button_select_sound();

            if (key == GLFW_KEY_DOWN)
            {
                buttonIndex = (buttonIndex + 1) % numButtons;
                set_currentButton(pauseButtons[buttonIndex]);
            }

            if (key == GLFW_KEY_UP)
            {
                int nextButton = buttonIndex - 1;
                if (nextButton < 0)
                    nextButton = numButtons - 1;
                buttonIndex = nextButton;
                set_currentButton(pauseButtons[buttonIndex]);
            }

            if (key == GLFW_KEY_ENTER)
            {
                Level* level = (Level*) game->get_state(LEVEL);
				soundManager->play_button_enter_sound();
                switch (currentButton->buttonName)
                {
                    case CONTINUE:
                        game->set_current_state(level);
                        break;
                    case RESTART:
                        level->reset_game();
                        game->set_current_state(level);
                        break;
                    case SAVE:
                    {
                        savePopup.set_visibility(true);
                        savePopup.set_popup_time();
                        GameSave::save_game(level);
                        MainMenuState* mainMenuState = (MainMenuState*) game->get_state(MAIN);
                        mainMenuState->show_load_button();
                        break;
                    }

                    case CONTROLS:
                        show_help_menu = true;
                        m_help_menu.set_visibility(show_help_menu);
                        break;
                    case MAIN_MENU:
                    {
                        MainMenuState* mainMenuState = (MainMenuState*) game->get_state(MAIN);
                        mainMenuState->reset_buttons();
                        game->set_current_state(mainMenuState);
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        if (key == GLFW_KEY_ESCAPE)
        {
            switch (currentButton->buttonName)
            {
                case CONTROLS:
                    show_help_menu = false;
                    m_help_menu.set_visibility(show_help_menu);
                    break;
                default:
                    break;
            }
        }

    }
}

bool PauseMenuState::is_over()
{
    return glfwWindowShouldClose(m_window);
}

void PauseMenuState::destroy()
{
    glDeleteFramebuffers(1, &m_frame_buffer);

    pauseMenu.destroy();
    savePopup.destroy();
    for (auto& button : pauseButtons) {
        button->destroy();
    }

}

void PauseMenuState::init_buttons()
{
    float buttonX = initialPosition.x;
    float buttonY = initialPosition.y - 50;
    float buttonOffset = 75.f;

    const char* continueText = textures_path("continue-button.png");
    const char* restartText = textures_path("restart-button.png");
    const char* saveText = textures_path("save-button.png");
    const char* controlsText = textures_path("controls-button.png");
    const char* menuText = textures_path("main-menu-button.png");
    continueButton.init(vec2({buttonX, buttonY}), continueText, CONTINUE );
    continueButton.set_selected(true);
    currentButton = &continueButton;
    restartButton.init(vec2({buttonX, buttonY + buttonOffset * 1}), restartText, RESTART );
    saveButton.init(vec2({buttonX, buttonY + buttonOffset * 2}), saveText, SAVE );
    controlsButton.init(vec2({buttonX, buttonY + buttonOffset * 3}), controlsText, CONTROLS);
    mainMenuButton.init(vec2({buttonX, buttonY + buttonOffset * 4}), menuText, MAIN_MENU);

    pauseButtons[0] = &continueButton;
    pauseButtons[1] = &restartButton;
    pauseButtons[2] = &saveButton;
    pauseButtons[3] = &controlsButton;
    pauseButtons[4] = &mainMenuButton;
}

void PauseMenuState::set_currentButton(MainButton* button)
{
    currentButton->set_selected(false);
    button->set_selected(true);
    currentButton = button;
}

void PauseMenuState::reset_buttons()
{
    buttonIndex = 0;
    set_currentButton(&continueButton);
}
