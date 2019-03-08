#include "../include/mazeComponents/mazeComponent.hpp"
#include <cmath>

// Put implementation common to all mazeComponents here

// Call if init() was successful
// Releases all graphics resources
void MazeComponent::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

/**
 * Retrieve window coordinates of maze component
 * @return coordinates of maze component
 */
vec2 MazeComponent::get_position()const {
    return m_position;
}

/**
 * Retrieve the scale of the maze component
 * @return scale of component
 */
vec2 MazeComponent::get_scale()const {
    return m_scale;
}

/**
 * Retrieve the angle that the maze component is rotated by
 * @return angle of maze component in radians
 */
float MazeComponent::get_rotation()const {
    return m_rotation;
}

void MazeComponent::set_position(vec2 position)
{
	m_position = position;
}

void MazeComponent::set_rotation(float radians)
{
	m_rotation = radians;
}

void MazeComponent::set_size(Texture* texture)
{
	m_width = std::fabs(m_scale.x) * texture->width;
	m_height = std::fabs(m_scale.y) * texture->height;
}

float MazeComponent::get_width()
{
	return m_width;
}
	
float MazeComponent::get_height()
{
	return m_height;
}

vec2 MazeComponent::get_bounding_box() const
{
	return { m_width, m_height };

}

float MazeComponent::get_drag()
{
    return drag;
}

void MazeComponent::set_vertex_coord()
{
	RenderManager::init_drawing_data(m_position, m_rotation, m_scale, this);

	for (auto vert : vertices)
	{
		mat3 max_mat{ {vert.position.x, vert.position.y, 1},
					  {0, 0, 1},
					  { 0, 0, 1 } };
		mat3 transformed = mul(transform, max_mat);

		vertex_coords.push_back(vec2({transformed.c0.x, transformed.c0.y}));
	}
}

std::vector<vec2> MazeComponent::get_vertex_coord()
{
	return vertex_coords;
}