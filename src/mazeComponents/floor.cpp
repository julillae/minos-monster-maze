#include "../include/mazeComponents/floor.hpp"

#include <cmath>

 Texture Floor::texture;

bool Floor::init(vec2 position)
{
    const char* textureFile = textures_path("platform.jpg");

    if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	set_position(position);

	m_rotation = 0.f;
	drag = 0.7;

	return true;
}

void Floor::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}


vec2 Floor::get_texture_size()
{
	return vec2({static_cast<float>(texture.width), static_cast<float>(texture.height)});
}
