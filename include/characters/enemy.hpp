#pragma once

#include "character.hpp"

// Generic Enemy Class
// Put methods specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here
class Enemy : public Character
{
	static Texture enemy_texture;

public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition, float bound);
	
	// Update enemy position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms)override;
	
	// Renders the enemy
	void draw(const mat3& projection)override;

	bool set_texture(const char* texturePath);

	// Freeze the enemy and unfreeze
	void freeze();
	void unfreeze();

	// Sets the enemy position to its initial position
	void reset_position();

	// gets bounding box of enemy
	vec2 get_bounding_box()const;


private:
	float speed = 2.0f;
	float stopBound;
	vec2 m_initialPosition;
	bool m_frozen;
};