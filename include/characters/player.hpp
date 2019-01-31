#pragma once

#include "character.hpp"
#include "enemy.hpp"
#include "../mazeComponents/fixedComponent.hpp"

class Enemy;
class FixedComponent;

class Player : public Character
{
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition);
	
	// Update player position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the player
	void draw(const mat3& projection)override;

	// Collision routines for enemies and fixed maze components
	//bool collides_with(const Turtle& turtle);
	//bool collides_with(const Fish& fish);

	// Set the player's acceleration vector
	void set_acceleration(vec2 acc);

	// Update player's velocity based on acceleration
	void update_velocity(double jumpVel);

	// Moves the player's position by the specified offset
	void move();

	// Change player's movement direction
	void set_direction(int key, int action);

	// Set player's rotation in radians
	void set_rotation(float radians);

	// True if the player is alive
	bool is_alive()const;

	// Kills the player, changing its alive state and triggering on death events
	void kill();


private:
	double tolerance = 0.000001;
	double maxVelocity = 12;
	double accStep = 0.6f;
	double maxAcceleration = 4;
	double drag = 0.95;
	vec2 currentVelocity;
	vec2 currentAcceleration;
	bool onPlatform;
	double fakeFloorPos = 500.f;
	double gravityAcc = 9.81 * 0.2;
	double jumpVel = -40.f;
};
