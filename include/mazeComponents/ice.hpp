#pragma once

#include "fixedComponent.hpp"

class Ice : public FixedComponent
{
    static Texture texture;

public:

    bool init(vec2 position)override;

    // Renders the component
	void draw(const mat3& projection)override;

	vec2 get_texture_size();
};

class Ices : public FixedComponents
{
public:
	bool spawn_ice(vec2 position);
	std::vector<Ice> get_ice_vector();

	// Renders the component
	void draw(const mat3& projection)override;

	void destroy();
private:
	std::vector<Ice> m_ices;
};