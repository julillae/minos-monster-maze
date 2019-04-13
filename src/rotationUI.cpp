#include "../include/rotationUI.hpp"

bool RotationUI::init(vec2 position)
{
    m_texture.id = 0;

    const char* textureFile = textures_path("rotation-bar.png");

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    if (!RenderManager::set_render_data(&m_texture, this)) return false;

    if (!effect.load_from_file(shader_path("visibility.vs.glsl"), shader_path("visibility.fs.glsl")))
        return false;

    m_rotation = 0.f;
    m_scale = {1.25f, 1.25f};
    set_visibility(true);
    set_position(position);

    return true;
}

void RotationUI::draw(const mat3& projection)
{
    RenderManager::draw_hide_texture(projection, m_position, m_rotation, m_scale, &m_texture, is_hidden, this);
}

void RotationUI::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;
}

void RotationUI::set_position(vec2 pos) {
    m_position = pos;
}