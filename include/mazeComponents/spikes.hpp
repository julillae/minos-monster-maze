#pragma once

#include "fixedComponent.hpp"
#include <vector>

class Spikes : public FixedComponent
{

public:

    // default spike points upward
    bool init(vec2 position)override;

    // Renders the component
    void draw(const mat3& projection)override;


    // Returns the bounding box for collision detection, called by collides_with()
    vec2 get_bounding_box()const;

    void set_dimensions();

    void set_left();
    void set_right();
    void set_down();
	void set_up();
};