#pragma once

#include "common.hpp"
#include "character.hpp"

// Salmon enemy 
class Turtle : public Character
{
	// Shared between all turtles, no need to load one for each instance
	static Texture turtle_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Update turtle due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Sets the new turtle position
	void set_position(vec2 position);

	// Returns the turtle' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

};