#include "../../include/menus/savePopup.hpp"

Texture SavePopup::texture;

bool SavePopup::init(vec2 position)
{
    const char* textureFile = textures_path("game-saved.png");

    if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

    if (!RenderManager::set_render_data(&texture, this)) return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("visibility.vs.glsl"), shader_path("visibility.fs.glsl")))
        return false;

    set_position(position);

    // Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 0.51f;
    m_scale.y = 0.51f;
    m_rotation = 0.f;

    return true;

}

void SavePopup::draw(const mat3 &projection)
{
    RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, color, is_hidden, this);
}

void SavePopup::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;

}

void SavePopup::set_popup_time()
{
    m_popup_time = glfwGetTime();
}

float SavePopup::get_time_since_popup()
{
    return glfwGetTime() - m_popup_time;
}






