#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class Button : public Renderable
{

public:

    void destroy();

    void set_position(vec2 position);

    void set_selected(bool isSelected);

    //button buttonName;
protected:
    vec2 m_position;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float m_rotation; // in radians
    bool isSelected = false;
    Texture m_texture;
};