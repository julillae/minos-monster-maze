#include "../include/mazeComponents/mazeComponent.hpp"
#include <cmath>

// Put implementation common to all mazeComponents here

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

