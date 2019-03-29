#include <math.h>
#include "../include/mazeComponents/fixedComponent.hpp"
#include <cmath>

// Put implementation specific to fixed maze components here

void FixedComponent::set_world_vertex_coord()
{
	RenderManager::init_drawing_data(m_position, m_rotation, m_scale, this);
	vertex_coords.clear();
	for (auto vert : local_vertex_coords)
	{
		mat3 max_mat{ {vert.x, vert.y, 1},
					  { 0, 0, 0 },
					  { 0, 0, 0 } };
		mat3 transformed = mul(transform, max_mat);

		vertex_coords.push_back(vec2({ transformed.c0.x, transformed.c0.y }));
	}
}

void FixedComponent::set_collision_properties()
{
	set_world_vertex_coord();
	boundingCircleRadius = sqrtf(static_cast<float>(pow(0.5 * m_width, 2.f) + pow(0.5 * m_height, 2.f)));
}

std::vector<vec2> FixedComponent::get_vertex_coord()
{
	return vertex_coords;
}