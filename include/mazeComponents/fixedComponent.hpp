#pragma once

#include "mazeComponent.hpp"

// subclass of mazeComponent for all objects unaffected by gravity
// examples include walls/platforms, spikes fixed to platforms, etc
// (I think because of rotation, we should just treat walls as platforms)

class FixedComponent : public MazeComponent
{

public:
    bool init(vec2 position) = 0;

	void draw(const mat3& projection) = 0;
};