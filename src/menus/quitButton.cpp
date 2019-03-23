#include "../../include/menus/quitButton.hpp"

Texture QuitButton::texture;

bool QuitButton::init(vec2 position)
{
    const char* textureFile = textures_path("quit-button.png");

    if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

    if (!RenderManager::set_render_data(&texture, this)) return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;

    set_position(position);

    // Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 1.f;
    m_scale.y = 1.f;
    m_rotation = 0.f;
    buttonName = QUIT;

    return true;

}

void QuitButton::draw(const mat3 &projection)
{
    draw_button(projection, &texture, isSelected);
}









