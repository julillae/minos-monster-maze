#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class MazeComponent : public Renderable
{
public:

	// Releases all associated resources
	void destroy();

	void draw(const mat3& projection) = 0;

	void set_size(vec2 size);

	// Sets the new position
	void set_position(vec2 position);

	// Set component rotation in radians
	void set_rotation(float radians);

	vec2 get_position()const;

	vec2 get_scale()const;

	float get_rotation()const;

	float get_width();
	
    float get_height();

protected:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	float m_width;
	float m_height;
};