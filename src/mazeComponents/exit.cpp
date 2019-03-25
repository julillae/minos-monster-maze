#include "../include/mazeComponents/exit.hpp"

#include <cmath>

Texture Exit::texture;

bool Exit::init(vec2 position)
{
	const char* textureFile = textures_path("exit.png");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

    set_position(position);

	// Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 0.2f;
	m_scale.y = 0.2f;
	m_rotation = 0.f;

    set_size();
	set_collision_properties();
	return true;
}

void Exit::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}

void Exit::set_size()
{
	m_width = std::fabs(m_scale.x) * texture.width;
	m_height = std::fabs(m_scale.y) * texture.height;
}