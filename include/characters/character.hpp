#pragma once

#include "../common.hpp"

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
	
	// Change character movement direction
	void set_direction(int key, int action);
	
	// Set character rotation in radians
	void set_rotation(float radians);

	// True if the character is alive
	bool is_alive()const;

	// Kills the character, changing its alive state and triggering on death events
	void kill();

protected:
	bool m_is_alive; // True if the character is alive
	Direction h_direction; // direction of horizontal movement
	Direction v_direction; // direction of vertical movement
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
};
