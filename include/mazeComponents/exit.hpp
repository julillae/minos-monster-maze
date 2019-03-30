#pragma once

#include "fixedComponent.hpp"

class Exit : public FixedComponent
{
    static Texture texture;

public:

    bool init(vec2 position)override;

    // Renders the component
	void draw(const mat3& projection)override;

    void set_size();
	void set_world_vertex_coord();
};