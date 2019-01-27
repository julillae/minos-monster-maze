#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{

public:
	// Sets the new enemy position
	void set_position(vec2 position);
};