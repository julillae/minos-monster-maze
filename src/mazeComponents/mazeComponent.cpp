#include "../include/mazeComponents/mazeComponent.hpp"

#include <cmath>

void MazeComponent::set_position(vec2 position)
{
	m_position = position;
}

void MazeComponent::set_rotation(float radians)
{
	m_rotation = radians;
}
