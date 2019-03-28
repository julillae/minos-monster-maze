#pragma once

#include "common.hpp"


class RenderEffects : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the water
	void draw(const mat3& projection)override;

	// Set dead time
	void set_player_dead();
	void set_level_complete_time();
	void set_rotation_end_time();
	void reset_player_dead_time();
	void reset_player_win_time();
	void reset_rotation_end_time();
	float get_time_since_death() const;
	float get_time_since_level_complete() const;
	float get_time_since_rotation_end()const;

private:
	// When salmon is alive, the time is set to -1
	float m_dead_time;
	float m_win_time;
	float m_rotation_end_time;
};
