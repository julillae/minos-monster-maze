#pragma once

#include "fixedComponent.hpp"
#include <vector>

class GeometricComponent : public FixedComponent
{

public:
    void set_dimensions();

protected:
	size_t m_num_indices; // passed to glDrawElements
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
};