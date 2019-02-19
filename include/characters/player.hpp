#pragma once

#include "character.hpp"
#include "enemy.hpp"
#include "../mazeComponents/fixedComponent.hpp"

class Enemy;
class FixedComponent;

class Player : public Character
{
	static Texture player_texture;
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition);
	
	// Update player position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the player
	void draw(const mat3& projection)override;

	// Returns the local bounding coordinates scaled by the current size of the fish 
	vec2 get_bounding_box()const;

	// Set the player's acceleration vector
	void set_acceleration(vec2 acc);

	// Get the player's acceleration vector
	vec2 get_acceleration();

	// Set the player's velocity vector
	void set_velocity(vec2 vel);

	// Get the player's velocity vector
	vec2 get_velocity();

	// Moves the player's position by the specified offset
	void move();

	// Let character know it's currently on a platform (and its position)
	void set_on_platform(float yPos);

	// Let character know it's currently in freefall
	void set_in_free_fall();

	// Change player's movement direction
	void set_direction(int key, int action);

	// Set player's rotation in radians
	void set_rotation(float radians);

	// True if the player is alive
	bool is_alive()const;

	// Kills the player, changing its alive state and triggering on death events
	void kill();

	bool isBelowPlatform;
	bool isLeftOfPlatform;
	bool isRightOfPlatform;
	float width;
	float height;
	float maxVelocity = 10;
	bool isOnPlatform;

private:
	float accStep = 1.f;
	float drag = 0.75;
	vec2 currentVelocity;
	vec2 currentAcceleration;
	float fakeFloorPos = 750.f;
	float currentFloorPos = fakeFloorPos;
	float gravityAcc = 9.81 * 0.12;
	float jumpVel = -18.f;
};
