#pragma once

#include "fixedComponent.hpp"

class Ice : public FixedComponent
{
public:

    bool init(vec2 position)override;

	bool set_texture_properties(Texture * texture);

    // Renders the component
	void draw(const mat3& projection)override;
};

class Ices : public FixedComponents
{
	static Texture texture;

public:
	bool renderSetup()override;
	bool spawn_ice(vec2 position);
	std::vector<Ice> get_ice_vector();

	// Renders the component
	void draw(const mat3& projection)override;

	void destroy();
	vec2 get_texture_size();
private:
	std::vector<Ice> m_ices;
};