#pragma once

#include "headerFiles/common.hpp"

class MazeComponent : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	virtual bool init() = 0;

	// Releases all associated resources
	void destroy();

	// Update component position
	// ms represents the number of milliseconds elapsed from the previous update() call
	virtual void update(float ms) = 0;

	// Renders the component
	void draw(const mat3& projection) = 0;

	// Set component rotation in radians
	void set_rotation(float radians);

protected:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
};