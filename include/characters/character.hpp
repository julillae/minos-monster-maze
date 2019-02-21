#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "../spriteSheet.hpp"

class Physics;

class Character : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	virtual bool init(vec2 initialPosition, Physics* physicsHandler);

	void set_properties(vec2 initialPosition, float scaleFactor, float xVel);

	// Releases all associated resources
	void destroy();

	// Update character position
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms) = 0;

	// Renders the character
	void draw(const mat3& projection) = 0;

	// Set the character's acceleration vector
	void set_acceleration(vec2 acc);

	// Get the character's acceleration vector
	vec2 get_acceleration();

	// Set the character's velocity vector
	void set_velocity(vec2 vel);

	// Get the character's velocity vector
	vec2 get_velocity();

	// Returns the current character position
	vec2 get_position()const;

	// Returns the current scale of the character
	vec2 get_scale()const;

	// Moves the character based on current velocity
	void move();

	// Change character movement direction
	void set_direction(Direction d);

	// Get character's horizontal direction
	Direction get_h_direction();
	
	// Set character rotation in radians
	void set_rotation(float radians);

	// True if the character is alive
	bool is_alive()const;

	// Kills the character, changing its alive state and triggering on death events
	void kill();

	float width;
	float height;

protected:
	Physics* physicsHandler;
	vec2 m_velocity;
	vec2 m_acceleration;
	vec2 m_position;
	vec2 m_scale;
	bool m_is_alive; // True if the character is alive
	bool m_is_collided; // True if character collided with object
	Direction direction; // direction of intended movement
    // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	Texture m_texture;
	SpriteSheet spriteSheet;
	float m_animTime = 0.f;
	bool is_anim_once = false;
};
