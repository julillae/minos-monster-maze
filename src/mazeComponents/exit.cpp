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
	set_world_vertex_coord();
	boundingCircleRadius = sqrtf(pow(0.5*m_width, 2.f) + pow(0.5*m_height, 2.f));
	return true;
}

void Exit::draw(const mat3& projection)
{
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &texture, this);
}

void Exit::set_size()
{
	m_width = std::fabs(m_scale.x) * texture.width * 0.25f;
	m_height = std::fabs(m_scale.y) * texture.height * 0.5f;
}

void Exit::set_world_vertex_coord()
{
	vertex_coords.clear();

	float rightSide = m_position.x + m_width / 2;
	float leftSide = m_position.x - m_width / 2;
	float topSide = m_position.y + m_height / 2;
	float bottomSide = m_position.y - m_height / 2;
	vec2 vert1b = { rightSide, topSide };
	vec2 vert2b = { rightSide, bottomSide };
	vec2 vert3b = { leftSide , bottomSide };
	vec2 vert4b = { leftSide, topSide };

	vertex_coords.push_back(vert1b);
	vertex_coords.push_back(vert2b);
	vertex_coords.push_back(vert3b);
	vertex_coords.push_back(vert4b);
}