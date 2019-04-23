#pragma once

#include "common.hpp"
#include "renderManager.hpp"

class RotationUI : public Renderable
{
public:
    bool init();

    void draw(const mat3& projection)override;

    void update(float percent);

    void set_visibility(bool show);

    void set_position(vec2 pos);

    void set_width();

    float get_width();

    void set_colour_mode(bool isLightMode);

private:
    Texture m_black_texture;
    Texture m_white_texture;
    bool is_hidden;
    float m_width;
    bool isLightMode;

};
