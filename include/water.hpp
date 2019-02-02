#pragma once

#include "common.hpp"


class Water : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the water
	void draw(const mat3& projection)override;

	// Set dead time
	void set_salmon_dead();
	void set_player_dead();
	void set_player_win();
	void reset_salmon_dead_time();
	void reset_player_win_time();
	float get_salmon_dead_time() const;
	float get_player_win_time() const;

private:
	// When salmon is alive, the time is set to -1
	float m_dead_time;
	float m_win_time;
};
