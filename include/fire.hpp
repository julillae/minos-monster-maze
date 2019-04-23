#pragma once

#include "common.hpp"
#include "renderManager.hpp"

class Fire : public Renderable
{

public:

    bool init(vec2 position);

	void draw(const mat3& projection)override;

};