#pragma once

#include "mazeComponent.hpp"

// subclass of mazeComponent for all objects unaffected by gravity
// examples include walls/platforms, spikes fixed to platforms, etc
// (I think because of rotation, we should just treat walls as platforms)

class FixedComponent : public MazeComponent
{

public:
	// sets the vertex coordinates of the object relative to the world
	void set_world_vertex_coord();

	void set_collision_properties();
	std::vector<vec2> get_vertex_coord();
};