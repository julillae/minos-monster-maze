#include "../../include/gameStates/mainMenuState.hpp"
#include "../../include/renderManager.hpp"
#include "../../include/gameStates/levelSelectState.hpp"
#include "../../include/gameStates/pauseMenuState.hpp"


// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>

MainMenuState::MainMenuState(Game *game)
{
    this->game = game;

}

void MainMenuState::init(vec2 screen)
{
    fprintf(stderr, "main menu init\n");
    this->m_window = game->m_window;
    name = MAIN;

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

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        fprintf(stderr, "Failed to open audio device");
        return;
    }
//
//    //Note: the following music credits needs to be added to a credit scene at the end of the game
//    //Secret Catacombs
//    //by Eric Matyas
//    //www.soundimage.org
//
    m_background_music = Mix_LoadMUS(audio_path("secret_catacombs.wav"));

    if (m_background_music == nullptr)
    {
        fprintf(stderr, "Failed to load sound\n %s,%s\n make sure the data directory is present",
                audio_path("salmon_dead.wav"),
                audio_path("secret_catacombs.wav"));
        return;
    }

    // Playing background music undefinitely
    Mix_PlayMusic(m_background_music, -1);
    Mix_VolumeMusic(50);

    fprintf(stderr, "Loaded music\n");

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    glViewport(0, 0, w, h);

    initialPosition = vec2({static_cast<float>(w / 2), static_cast<float>(h / 2)});

    mainMenu.init(initialPosition);
    m_help_menu.init(initialPosition);
    m_help_menu.set_visibility(false);
    init_buttons();
    initialize_camera_position(w, h);
    mainMenu.set_position(cameraCenter);

    LevelSelectState* levelSelect = new LevelSelectState(game);
    levelSelect->init(m_screen);
    game->push_state(levelSelect);

    PauseMenuState* pauseMenu = new PauseMenuState(game);
    pauseMenu->init(m_screen);
    game->push_state(pauseMenu);

}

void MainMenuState::draw()
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

    mat3 projection_2D = calculate_projection(osScaleFactor);

    mainMenu.draw(projection_2D);
    loadButton.draw(projection_2D);
    newGameButton.draw(projection_2D);
    controlsButton.draw(projection_2D);
    quitButton.draw(projection_2D);
    m_help_menu.draw(projection_2D);
    // Presenting
    glfwSwapBuffers(m_window);
}

bool MainMenuState::update(float elapsed_ms)
{
    return true;
}

void MainMenuState::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    if (action == GLFW_PRESS) {
        if (!show_help_menu)
        {
            if (key == GLFW_KEY_ENTER)
            {
                switch (currentButton->buttonName)
                {
                    case NEWGAME:
                    {
                        LevelSelectState* levelSelectState = (LevelSelectState*) game->get_state(LEVELSELECT);
                        levelSelectState->reset_buttons();
                        game->set_current_state(levelSelectState);
                        break;
                    }
                    case CONTROLS:
                        show_help_menu = true;
                        m_help_menu.set_visibility(show_help_menu);
                        break;
                    case QUIT:
                    {
                        close = true;
                        break;
                    }
                    case LOAD:
                    {
                        Level* level = (Level*) game->get_state(LEVEL);
                        GameSave::load_game();
                        int startLevel = GameSave::document["level"].GetInt();

                        if (level == NULL)
                        {
                            Physics *physicsHandler = new Physics();
                            level = new Level(game);
                            level->init(m_screen, physicsHandler, startLevel);
                            level->load_saved_game();
                            game->push_state(level);
                        } else {
                            level->load_select_level(startLevel);
                            level->load_saved_game();
                        }
                        game->set_current_state(level);
                        world = level;
                        break;
                    }
                    default:
                        break;
                }

            }

            if (key == GLFW_KEY_DOWN)
            {
                buttonIndex = (buttonIndex + 1) % numButtons;
                set_currentButton(mainButtons[buttonIndex]);
            }


            if (key == GLFW_KEY_UP)
            {
                int nextButton = buttonIndex - 1;
                if (nextButton < 0)
                    nextButton = numButtons - 1;
                buttonIndex = nextButton;
                set_currentButton(mainButtons[buttonIndex]);
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

bool MainMenuState::is_over()
{
    return glfwWindowShouldClose(m_window) || close;
}

void MainMenuState::destroy()
{
    glDeleteFramebuffers(1, &m_frame_buffer);

    if (m_background_music != nullptr)
        Mix_FreeMusic(m_background_music);

    Mix_CloseAudio();

    mainMenu.destroy();
    loadButton.destroy();
    newGameButton.destroy();
    controlsButton.destroy();
    quitButton.destroy();
    m_help_menu.destroy();

}

void MainMenuState::init_buttons()
{
    float buttonX = initialPosition.x;
    float buttonY = initialPosition.y + 40;
    float buttonOffset = 75.f;

    const char* continueText = textures_path("load-button.png");
    const char* newGameText = textures_path("new-game-button.png");
    const char* controlsText = textures_path("controls-button.png");
    const char* quitText = textures_path("quit-button.png");
    loadButton.init(vec2({buttonX, buttonY}), continueText, LOAD );
    newGameButton.init(vec2({buttonX, buttonY + buttonOffset}), newGameText, NEWGAME);

    std::ifstream infile("../src/savedGame.txt");
    if (infile.good()) {
        loadButton.set_visibility(true);
        loadButton.set_selected(true);
        currentButton = &loadButton;
        show_continue = true;
        saved_file = true;
        numButtons = 4;
        buttonIndex = 3;
    } else {
        loadButton.set_visibility(false);
        newGameButton.set_selected(true);
        currentButton = &newGameButton;
        saved_file = false;
        numButtons = 3;
        buttonIndex = 0;
    }

    controlsButton.init(vec2({buttonX, buttonY + buttonOffset * 2}), controlsText, CONTROLS);
    quitButton.init(vec2({buttonX, buttonY + buttonOffset * 3}), quitText, QUIT);

    mainButtons[0] = &newGameButton;
    mainButtons[1] = &controlsButton;
    mainButtons[2] = &quitButton;
    mainButtons[3] = &loadButton;

    infile.close();

}

void MainMenuState::set_currentButton(MainButton* button)
{
    currentButton->set_selected(false);
    button->set_selected(true);
    currentButton = button;
}

void MainMenuState::reset_buttons()
{
    if (saved_file) {
        buttonIndex = 3;
        set_currentButton(&loadButton);
    } else {
        buttonIndex = 0;
        set_currentButton(&newGameButton);
    }
}

void MainMenuState::show_load_button()
{
    std::ifstream infile("../src/savedGame.txt");
    if (infile.good()) {
        saved_file = true;
        loadButton.set_visibility(true);
        numButtons = 4;
    }
    infile.close();
}
