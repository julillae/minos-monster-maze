#include "../../include/gameStates/levelSelectState.hpp"

mat3 GameState::calculate_projection(float osScaleFactor)
{
    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w;// *0.5;
    float bottom = (float)h;// *0.5;

    float sx = 2.f * osScaleFactor / (right - left);
    float sy = 2.f * osScaleFactor / (top - bottom); //this is where you play around with the camera

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

    return projection_2D;
}

void GameState::set_onKey()
{
    glfwSetWindowUserPointer(m_window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GameState*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    glfwSetKeyCallback(m_window, key_redirect);
}

void GameState::initialize_camera_position(int w, int h)
{
    float txOffset = w / 2;
    float tyOffset = h / 2;
    cameraCenter = vec2({ txOffset, tyOffset});
}

void GameState::render_to_framebuffer_screen()
{
    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    render_to_framebuffer(w, h);
    render_to_screen(w, h);
}

void GameState::render_to_framebuffer(int w, int h)
{

    // First render to the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
    const float clear_color[3] = { 0.5f, 0.5f, 0.5f };

    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_to_screen(w, h);
}

void GameState::render_to_screen(int w, int h)
{
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
}