#pragma once

#include "common.hpp"

class SpriteSheet
{

public:

    void init(Texture* texture, const vec2 spriteDims);

    bool set_render_data(Renderable *renderable, int index);

    Texture* texture;
    vec2 dims;

};