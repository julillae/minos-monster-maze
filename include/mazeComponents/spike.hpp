#pragma once

#include "geometricComponent.hpp"
#include <vector>

enum SpikeDir { UP, DOWN, LEFT, RIGHT };

class Spike : public GeometricComponent
{

public:

    // default spike points upward
    bool init(vec2 position)override;

    // Renders the component
    void draw(const mat3& projection)override;
};

class Spikes : public GeometricComponents
{
public:
	bool spawn_spike(vec2 position, SpikeDir dir);
	std::vector<Spike> get_spike_vector();

	// Renders the component
	void draw(const mat3& projection)override;

	void destroy();
	bool renderSetup()override;
private:
	std::vector<Spike> m_spikes;
};