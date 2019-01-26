#pragma once

#include "common.hpp"
#include "character.hpp"

class Turtle;
class Fish;

class Salmon : public Character
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);
	
	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);

	// Returns the current salmon position
	vec2 get_position()const;

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// Change salmon movement direction
	void set_direction(int key, int action);

	// Rotate salmon to look at mouse
	void look_at_mouse(double mX, double mY);

	// Set salmon rotation in radians
	void set_rotation(float radians);

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Called when the salmon collides with a fish, starts lighting up the salmon
	void light_up();

	// Called when the user switches advanced_mode on and off
	void set_advanced_mode(bool toggle);

private:
	bool advanced_mode; // toggle for "advanced" salmon behaviour (creative part of assignment)
	float max_left_limit; // used for determining salmon swimming boundaries
	float max_right_limit; // used for determining salmon swimming boundaries
	float max_top_limit; // used for determining salmon swimming boundaries
	float max_bottom_limit; // used for determining salmon swimming boundaries
	float bounce_down_countdown_ms; // used for boundary bounce mechanic
	float bounce_up_countdown_ms; // used for boundary bounce mechanic
	float bounce_left_countdown_ms; // used for boundary bounce mechanic
	float bounce_right_countdown_ms; // used for boundary bounce mechanic
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	//bool m_is_alive; // True if the salmon is alive
	//Direction h_direction; // direction of horizontal movement
	//Direction v_direction; // direction of vertical movement
	//vec2 m_position; // Window coordinates
	//vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	//float m_rotation; // in radians
	//size_t m_num_indices; // passed to glDrawElements
};
