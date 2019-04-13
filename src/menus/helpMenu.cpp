#include "../../include/menus/helpMenu.hpp"

Texture HelpMenu::texture;

bool HelpMenu::init(vec2 position)
{
    const char* textureFile = textures_path("help-menu.png");

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

void HelpMenu::draw(const mat3 &projection)
{
    RenderManager::draw_hide_texture(projection, m_position, m_rotation, m_scale, &texture, is_hidden, this);
}

void HelpMenu::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;

}





