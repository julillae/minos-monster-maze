#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{
	static Texture fish_texture;

public:
	// Sets the new enemy position
	void set_position(vec2 position);

	// Creates all the associated render resources and default transform
	bool init();
	
	// Update enemy position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the enemy
	void draw(const mat3& projection)override;

	// Collision routines for enemies and fixed maze components
	//bool collides_with(const Turtle& turtle);
	//bool collides_with(const Fish& fish);

	// Moves the enemy's position by the specified offset
	void move();

	// Change enemy's movement direction
	void set_direction(int key, int action);

	// Set enemy's rotation in radians
	void set_rotation(float radians);

	// True if the enemy is alive
	bool is_alive()const;

	// Kills the enemy, changing its alive state and triggering on death events
	void kill();


private:
	double tolerance = 0.000001;
	double maxVelocity = 12;
	vec2 currentVelocity;
};