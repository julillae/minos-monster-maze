#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class Button : public Renderable
{

public:

    void destroy();

    void set_position(vec2 position);

    void set_selected(bool isSelected);

protected:
    bool isSelected = false;
    Texture m_texture;
    bool is_hidden = false;
};