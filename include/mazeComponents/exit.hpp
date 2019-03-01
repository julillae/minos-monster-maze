#pragma once

#include "fixedComponent.hpp"

class Exit : public FixedComponent
{
    static Texture texture;

public:

    bool init(vec2 position);

    // Renders the component
	void draw(const mat3& projection)override;

	// Returns the bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

    void set_size();
};