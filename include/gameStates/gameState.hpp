#pragma once

#include "../game.hpp"

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

    void set_onKey() {
        glfwSetWindowUserPointer(m_window, this);
        auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GameState*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
        glfwSetKeyCallback(m_window, key_redirect);
    }

protected:
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

    vec2 m_position;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float m_rotation; // in radians
    vec2 m_screen;
};