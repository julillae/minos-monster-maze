#pragma once

#include "../common.hpp"
#include "../textureManager.hpp"
#include "../spriteSheet.hpp"

class Character : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	virtual bool init(vec2 initialPosition) = 0;

	// Releases all associated resources
	void destroy();

	// Update character position
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms) = 0;

	// Renders the character
	void draw(const mat3& projection) = 0;

	// Collision routines for other characters and fish
	//bool collides_with(const Character& character);
	//bool collides_with(const Fish& fish);

	// Returns the current character position
	vec2 get_position()const;

	// Returns the current scale of the character
	vec2 get_scale()const;

	// Moves the character's position by the specified offset
	void move(vec2 off);

	// Let character know it's currently on a platform (and its position)
	void set_on_platform(double yPos);

	// Change character movement direction
	void set_direction(int key, int action);
	
	// Set character rotation in radians
	void set_rotation(float radians);

	// True if the character is alive
	bool is_alive()const;

	// Kills the character, changing its alive state and triggering on death events
	void kill();


protected:
	vec2 m_position;
	vec2 m_scale;
	bool m_is_alive; // True if the character is alive
	bool m_is_collided; // True if character collided with object
	Direction h_direction; // direction of horizontal movement
	Direction v_direction; // direction of vertical movement
    // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	Texture m_texture;
	SpriteSheet spriteSheet;
	float m_animTime = 0.f;
	bool is_anim_once = false;
};
