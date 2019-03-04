#include "../include/characters/smart.hpp"

// Put shared implementation for smart enemies here

bool Smart::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;

	set_properties(initialPosition, 0.4f, speed);
	direction = Direction::right;
	m_initialPosition = initialPosition;

	return true;
}

void Smart::update(float ms)
{

	if (is_alive())
	{
		// Update enemy position to follow the player using enemy AI
		
	}
	else
	{
		// If dead reset do something
	}

}

void Smart::set_animation()
{
    // TODO: add animation 
}