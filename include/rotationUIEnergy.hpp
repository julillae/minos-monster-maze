#pragma once

#include "common.hpp"
#include "renderManager.hpp"

class RotationUIEnergy : public Renderable
{
public:
    bool init(vec2 position);

    void draw(const mat3& projection)override;

    void update(float energyLevel);

    void set_visibility(bool show);

    void set_position(vec2 pos);

    void set_energy_level(float energyLevel);

private:
    Texture m_texture;
    bool is_hidden;
    vec2 original_position;
    float original_leftEdge;
    float energyLevel;

};
