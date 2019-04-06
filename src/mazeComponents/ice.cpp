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

Texture Ices::texture;

bool Ices::renderSetup()
{
	const char* textureFile = textures_path("ice2.png");

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

bool Ices::spawn_ice(vec2 position)
{
	Ice ice;

	if (ice.init(position))
	{
		ice.set_scale(m_scale);
		ice.set_size(m_size);
		ice.set_collision_properties();
		m_ices.emplace_back(ice);
		return true;
	}
	fprintf(stderr, "Failed to spawn ice");
	return false;
}

std::vector<Ice> Ices::get_ice_vector()
{
	return m_ices;
}

void Ices::draw(const mat3 & projection)
{
	for (auto& ice : m_ices)
	{
		vec2 position = ice.get_position();
		float rotation = ice.m_rotation;
		vec2 scale = ice.m_scale;
		RenderManager::draw_texture(projection, position, rotation, scale, &texture, this);
	}
}

void Ices::destroy()
{
	for (auto& ice : m_ices)
		ice.destroy();
	m_ices.clear();
}

vec2 Ices::get_texture_size()
{
	return vec2({ static_cast<float>(texture.width), static_cast<float>(texture.height) });
}
