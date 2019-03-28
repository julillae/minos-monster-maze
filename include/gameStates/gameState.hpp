#pragma once

#include "../game.hpp"
#include "../common.hpp"

class GameState
{
public:
    Game* game;

    virtual void draw() = 0;
    // Steps the game ahead by ms milliseconds
    virtual bool update(float elapsed_ms) = 0;
    virtual void on_key(GLFWwindow*, int key, int, int action, int mod) = 0;
    virtual bool is_over() = 0;
    virtual void destroy() = 0;

    void set_onKey();
    void initialize_camera_position(int w, int h);

    mat3 calculate_projection();
    void render_to_framebuffer_screen();

    gameStates name;

protected:
    void render_to_screen(int w, int h);
    void render_to_framebuffer(int w, int h);

    // Window handle
    GLFWwindow* m_window;

    // Screen texture
    // The draw loop first renders to this texture, then it is used for the water shader
    GLuint m_frame_buffer;
    Texture m_screen_tex;

// Part of hack needed to deal with the MacOs Retina Display issue where it doubles the pixels rendered
    float osScaleFactor = 1.f;

    float tx;
    float ty;
    vec2 cameraCenter;

    vec2 m_screen;
};