#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{

public:
	// Creates all the associated render resources and default transform
	virtual bool init(vec2 initialPosition, Physics* physicsHandler);
	
	// Update enemy position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms);
	
	// Renders the enemy
	void draw(const mat3& projection)override;

	// Freeze the enemy and unfreeze
	void freeze();
	void unfreeze();

	// Sets the enemy position to its initial position
	void reset_position();

	// gets bounding box of enemy
	vec2 get_bounding_box()const;


protected:
	float speed = 2.0f;
	vec2 m_initialPosition;
	bool m_frozen;
};