#pragma once

#include "character.hpp"

class Player : public Character
{
	static Texture player_texture;
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition, Physics* physicsHandler)override;
	
	// Update player position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the player
	void draw(const mat3& projection)override;

	// Returns the local bounding coordinates scaled by the current size of the fish 
	vec2 get_bounding_box()const;

	// Set the player's acceleration vector
	//void set_acceleration(vec2 acc);

	// Set the player's velocity vector
	//void set_velocity(vec2 vel);

	// Moves the player's position by the specified offset
	void move();

	// Let character know it's currently on a platform (and its position)
	void set_on_platform(float yPos);

	// Let character know it's currently in freefall
	void set_in_free_fall();

	// Change player's movement direction
	void set_direction(int key, int action);

	bool isBelowPlatform;
	bool isLeftOfPlatform;
	bool isRightOfPlatform;
	float width;
	float height;
	float maxVelocity = 10;
	bool isOnPlatform;
	float accStep = 1.f;

private:
	float jumpVel = -18.f;
};
