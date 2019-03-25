#pragma once

#include "geometricComponent.hpp"
#include <vector>

class Spikes : public GeometricComponent
{

public:

    // default spike points upward
    bool init(vec2 position)override;

    // Renders the component
    void draw(const mat3& projection)override;
	
    void set_left();
    void set_right();
    void set_down();
	void set_up();
};