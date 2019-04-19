#include "../../include/gameStates/creditsState.hpp"
#include "../../include/renderManager.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>

CreditsState::CreditsState(Game *game)
{
    this->game = game;

}

void CreditsState::init(vec2 screen)
{
    fprintf(stderr, "pause menu init\n");
    this->m_window = game->m_window;
    name = CREDITS;

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

    credits.init(initialPosition);
    m_help_menu.init(initialPosition);
    m_help_menu.set_visibility(false);
    initialize_camera_position(w, h);
    credits.set_position(cameraCenter);

}

void CreditsState::draw()
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

    credits.draw(projection_2D);

    m_help_menu.draw(projection_2D);
    // Presenting
    glfwSwapBuffers(m_window);
}

bool CreditsState::update(float elapsed_ms)
{
    vec2 currPos = credits.get_position();
    float texture_height = -350.f;

    if (currPos.y >= texture_height) {
        float increment = 0.5;
        credits.set_position({currPos.x, currPos.y - increment});
    } else {
        // close game
        close = true;
    }

    return true;
}

void CreditsState::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    
}

bool CreditsState::is_over()
{
    return glfwWindowShouldClose(m_window) || close;
}

void CreditsState::destroy()
{
    glDeleteFramebuffers(1, &m_frame_buffer);

    credits.destroy();
}

