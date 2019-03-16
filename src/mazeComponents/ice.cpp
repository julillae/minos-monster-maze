#include "../include/mazeComponents/ice.hpp"

#include <cmath>

 Texture Ice::texture;

bool Ice::init(vec2 position)
{
	const char* textureFile = textures_path("ice2.png");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	set_position(position);

	m_rotation = 0.f;
	drag = 0.96f;

    return true;
}


void Ice::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}

vec2 Ice::get_texture_size()
{
	return vec2({static_cast<float>(texture.width), static_cast<float>(texture.height)});
}