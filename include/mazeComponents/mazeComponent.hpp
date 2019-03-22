#pragma once

#include <vector>
#include "../common.hpp"
#include "../renderManager.hpp"

class MazeComponent : public Renderable
{
public:

    virtual bool init(vec2 position) = 0;

	// Releases all associated resources
	void destroy();

	virtual void draw(const mat3& projection) = 0;

	void set_size(vec2 size);

	// Sets the new position
	void set_position(vec2 position);

	// Set component rotation in radians
	void set_rotation(float radians);

	vec2 get_position()const;

	void set_scale(vec2 scale);

	float get_rotation()const;

	float get_width();
	
    float get_height();

    vec2 get_bounding_box()const;

    float get_drag();

    bool can_kill = false;

	float boundingCircleRadius;
protected:
	float m_width;
	float m_height;
    float drag;
};