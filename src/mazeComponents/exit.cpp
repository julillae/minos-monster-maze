#include "../include/mazeComponents/exit.hpp"

#include <cmath>

Texture Exit::texture;

bool Exit::init(vec2 position)
{
	const char* textureFile = textures_path("exit.png");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

    set_position(position);

	// Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 0.2f;
	m_scale.y = 0.2f;
	m_rotation = 0.f;

    set_size();

	return true;
}

void Exit::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}


// Returns the local bounding coordinates scaled by the current size of the component
vec2 Exit::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * texture.width, std::fabs(m_scale.y) * texture.height };
}

void Exit::set_size()
{
	m_width = std::fabs(m_scale.x) * texture.width;
	m_height = std::fabs(m_scale.y) * texture.height;
}