#include "../include/mazeComponents/floor.hpp"

#include <cmath>

bool Floor::init(vec2 position)
{
	set_position(position);
	m_rotation = 0.f;
	drag = 0.7;
	return true;
}

bool Floor::set_texture_properties(Texture * texture)
{
	if (!RenderManager::set_vertex_data(texture, this)) return false;
	return true;
}

void Floor::draw(const mat3& projection)
{
	return;
}

Texture Floors::texture;

bool Floors::renderSetup()
{
	const char* textureFile = textures_path("platform.jpg");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	vec2 textureSize = get_texture_size();
	float x_scale = m_tile_width / textureSize.x;
	float y_scale = m_tile_height / textureSize.y;
	m_scale = vec2({ x_scale, y_scale });
	
	return true;
}

bool Floors::spawn_floor(vec2 position)
{
	Floor floor;

	if (floor.init(position))
	{
		floor.set_texture_properties(&texture);
		floor.set_scale(m_scale);
		floor.set_size(m_size);
		floor.set_collision_properties();
		m_floors.emplace_back(floor);
		return true;
	}
	fprintf(stderr, "Failed to spawn floor");
	return false;
}

std::vector<Floor> Floors::get_floor_vector()
{
	return m_floors;
}

void Floors::draw(const mat3 & projection)
{
	for (auto& floor : m_floors)
	{
		vec2 position = floor.get_position();
		float rotation = floor.m_rotation;
		RenderManager::draw_texture(projection, position, rotation, m_scale, &texture, this);
	}
}

void Floors::destroy()
{
	for (auto& floor : m_floors)
		floor.destroy();
	m_floors.clear();
}

vec2 Floors::get_texture_size()
{
	return vec2({ static_cast<float>(texture.width), static_cast<float>(texture.height) });
}
