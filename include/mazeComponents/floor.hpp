#pragma once

#include "fixedComponent.hpp"

class Floor : public FixedComponent
{
    static Texture texture;

public:

    bool init(vec2 position)override;

    // Renders the component
	void draw(const mat3& projection)override;

	vec2 get_texture_size();

};

class Floors : public FixedComponents
{
public:
	bool init(float tile_width, float tile_height);
	bool spawn_floor(vec2 position);
	std::vector<Floor> get_floor_vector();

	// Renders the component
	void draw(const mat3& projection)override;

	void destroy();
private:
	float m_tile_width;
	float m_tile_height;
	std::vector<Floor> m_floors;
};