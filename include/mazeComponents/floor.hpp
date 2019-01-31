#pragma once

#include "fixedComponent.hpp"

class Floor : public FixedComponent
{
    static Texture texture;

public:

    bool init();

    // Renders the component
	void draw(const mat3& projection)override;

    bool set_texture();

    // Returns the bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

protected:
    void setSize(vec2 size);
};