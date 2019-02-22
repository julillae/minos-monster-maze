#include "../include/mazeComponents/floor.hpp"

#include <cmath>

 Texture Floor::texture;

bool Floor::init(vec2 position)
{
    const char* textureFile = textures_path("platform.jpg");

    if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

    if (!RenderManager::set_render_data(&texture, this)) return false;

	set_position(position);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
    m_scale.x = 25.0f / texture.width;  //texture.width scale
	m_scale.y = 25.0f / texture.height; //texture.height scale
	m_rotation = 0.f;

	set_size();

	return true;
}

void Floor::draw(const mat3& projection)
{
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &texture, this);
}

vec2 Floor::get_position()const
{
	return m_position;
}


// Returns the local bounding coordinates scaled by the current size of the component
vec2 Floor::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * texture.width, std::fabs(m_scale.y) * texture.height };
}

void Floor::set_size()
{
	m_width = std::fabs(m_scale.x) * texture.width;
	m_height = std::fabs(m_scale.y) * texture.height;
}
