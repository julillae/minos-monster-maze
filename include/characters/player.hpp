#pragma once

#include "character.hpp"

class Player : public Character
{
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition, Physics* physicsHandler)override;
	
	// Update player position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms)override;
	
	// Renders the player
	void draw(const mat3& projection)override;

	// Returns the local bounding coordinates scaled by the current size of the fish 
	vec2 get_bounding_box()const;

	// Let character know it's currently on a platform (and its position)
	void set_on_platform();

	// Let character know it's currently in freefall
	void set_in_free_fall();

	// Change player's state based on keyboard input
	void on_key(int key, int action);

	// Sets the correct sprite sheet tile for appropriate animation
	void set_animation()override;

	// Returns true if the character is in a state that can jump
	bool can_jump();

	bool keyMappingSetA=true;
	int jumpKey = GLFW_KEY_UP;

};
