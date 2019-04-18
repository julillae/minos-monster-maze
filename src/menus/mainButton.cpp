#include "../../include/menus/mainButton.hpp"


bool MainButton::init(vec2 position, const char* textureFile, button name)
{

    m_texture.id = 0;

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    if (!RenderManager::set_render_data(&m_texture, this)) return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("visibility.vs.glsl"), shader_path("visibility.fs.glsl")))
        return false;

    set_position(position);

    // Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 0.7f;
    m_scale.y = 0.7f;
    m_rotation = 0.f;
    buttonName = name;
    is_hidden = false;

    return true;

}

void MainButton::draw(const mat3 &projection)
{
    if (isSelected) {
        float color[] = {0.f, 0.f, 0.f};
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &m_texture, color, is_hidden, this);
    } else {
        float color[] = {1.f, 1.f, 1.f};
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &m_texture, color, is_hidden, this);
    }
}

void MainButton::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;
}











