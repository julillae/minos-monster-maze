#include "../include/mazeComponents/fixedComponent.hpp"

// Put implementation specific to fixed maze components here

void FixedComponent::set_vertex_coord()
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
	//terrible hack to set radius at this time,
	//but this is where we're sure that the width and height have been properly set
	//needed because code to set mazeComponent dimensions is all over the place
	//eventually will refactor this out
	boundingCircleRadius = sqrtf(pow(0.5*m_width, 2.f) + pow(0.5*m_height, 2.f));
}

std::vector<vec2> FixedComponent::get_vertex_coord()
{
	return vertex_coords;
}