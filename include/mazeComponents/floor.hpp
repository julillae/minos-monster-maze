#pragma once

#include "fixedComponent.hpp"

class Floor : public FixedComponent
{
    static Texture texture;

public:

    bool init(vec2 position)override;

    // Renders the component
	void draw(const mat3& projection)override;

};

class Floors : public FixedComponents
{
	static Texture texture;

public:
	bool renderSetup();
	bool spawn_floor(vec2 position);
	std::vector<Floor> get_floor_vector();

	// Renders the component
	void draw(const mat3& projection)override;

	void destroy();
	vec2 get_texture_size();
private:
	std::vector<Floor> m_floors;
};