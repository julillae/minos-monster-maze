#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{
	static Texture enemy_texture;

public:
	// Sets the new enemy position
	void set_position(vec2 position);

	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition, float bound);

	bool init(vec2 initialPosition);
	
	// Update enemy position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the enemy
	void draw(const mat3& projection)override;

	// Collision routines for enemies and fixed maze components
	//bool collides_with(const Turtle& turtle);
	//bool collides_with(const Fish& fish);

	bool set_texture(const char* texturePath);

	// Moves the enemy's position by the specified offset
	void move();

	// Freeze the enemy and unfreeze
	void freeze();
	void unfreeze();

	// Change enemy's movement direction
	void set_direction(Direction direction);

	// Set enemy's rotation in radians
	void set_rotation(float radians);

	// Sets the enemy position to its initial position
	void reset_position();

	// True if the enemy is alive
	bool is_alive()const;

	// Kills the enemy, changing its alive state and triggering on death events
	void kill();

	// gets bounding box of enemy
	vec2 get_bounding_box()const;


private:
	double tolerance = 0.000001;
	float maxVelocity = 2.0f;
	vec2 currentVelocity;
	Direction direction;
	float stopBound;
	vec2 m_initialPosition;
	bool m_frozen;
};