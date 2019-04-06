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

Texture Floors::texture;

bool Floors::renderSetup()
{
	const char* textureFile = textures_path("platform.jpg");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	return true;
}

bool Floors::spawn_floor(vec2 position)
{
	Floor floor;

	if (floor.init(position))
	{
		vec2 textureSize = get_texture_size();
		float x_scale = m_tile_width / textureSize.x;
		float y_scale = m_tile_height / textureSize.y;
		floor.set_scale(vec2({ x_scale, y_scale }));
		floor.set_size(vec2({ m_tile_width, m_tile_height }));
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
		vec2 scale = floor.m_scale;
		RenderManager::draw_texture(projection, position, rotation, scale, &texture, this);
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
