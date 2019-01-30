#pragma once

#include "../common.hpp"

class MazeComponent : public Renderable
{
public:
	// Shared between all maze components, no need to load one for each instance
	Texture texture;

	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the component
	void draw(const mat3& projection)override;

	// Returns the bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	// Sets the new position
	void set_position(vec2 position);

	// Set component rotation in radians
	void set_rotation(float radians);

protected:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
};