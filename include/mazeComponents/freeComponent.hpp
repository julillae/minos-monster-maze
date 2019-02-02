#pragma once

#include "mazeComponent.hpp"

// subclass of mazeComponent for all objects affected by gravity
// Examples include falling spikes or rolling boulders

class FreeComponent : public MazeComponent
{
    public:

	void update(float ms);
	
	// Returns the bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;
	
};