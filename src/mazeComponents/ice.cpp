#include "../include/mazeComponents/ice.hpp"

#include <cmath>

 Texture Ice::texture;

bool Ice::init(vec2 position)
{
	const char* textureFile = textures_path("ice2.png");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	set_position(position);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
    m_scale.x = 25.0f / texture.width;
	m_scale.y = 25.0f / texture.height;
	m_rotation = 0.f;
	drag = 0.94f;

	set_size(&texture);

    return true;
}


void Ice::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}