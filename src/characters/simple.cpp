#include "../include/characters/simple.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put shared implementation for simple enemies here

Texture Simple::simple_texture;

bool Simple::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;
	direction = right;
	m_stopBound = 10.f;
	m_initialPosition = initialPosition;

	return true;
}

void Simple::update(float ms)
{

	if (m_is_alive)
	{
		// Update enemy position based on fixed path here
		float vel_x = m_velocity.x;
		float vel_y = m_velocity.y;
		float right_bound = m_initialPosition.x + m_stopBound;
		float left_bound = m_initialPosition.x;
		float next_pos_x = m_position.x + vel_x;

		if (((vel_x > 0) && (next_pos_x >= right_bound)) ||
			((vel_x < 0) && (next_pos_x <= left_bound))) {
			vel_x *= -1;
			m_scale.x *= -1;
			set_velocity({ vel_x, vel_y });
		}
		move();
	}

}


void Simple::set_bound(float bound) 
{
   m_stopBound = bound;
}