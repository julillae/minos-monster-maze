#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{

public:

	// Sets the enemy position to its initial position
	void reset_position();

	// gets bounding box of enemy
	vec2 get_bounding_box()const;


protected:
	float speed = 2.0f;
	vec2 m_initialPosition;
};