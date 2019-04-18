#include "../include/mazeComponents/ice.hpp"

#include <cmath>

bool Ice::init(vec2 position)
{
	set_position(position);
	m_rotation = 0.f;
	drag = 0.96f;
    return true;
}

bool Ice::set_texture_properties(Texture * texture)
{
	if (!RenderManager::set_vertex_data(texture, this)) return false;
	return true;
}

void Ice::draw(const mat3& projection)
{
	return;
}

Texture Ices::texture;

bool Ices::renderSetup()
{
	const char* textureFile = textures_path("ice2.png");

	if (!RenderManager::load_texture(textureFile, &texture, this)) return false;

	if (!RenderManager::set_render_data(&texture, this)) return false;

	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	float x_scale = m_tile_width / static_cast<float>(texture.width);
	float y_scale = m_tile_height / static_cast<float>(texture.height);
	m_scale = vec2({ x_scale, y_scale });

	return true;
}

bool Ices::spawn_ice(vec2 position)
{
	Ice ice;

	if (ice.init(position))
	{
		ice.set_texture_properties(&texture);
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
		RenderManager::draw_texture(projection, position, rotation, m_scale, &texture, color, is_hidden, this);
	}
}

void Ices::destroy()
{
	for (auto& ice : m_ices)
		ice.destroy();
	m_ices.clear();
}
