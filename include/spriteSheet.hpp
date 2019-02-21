#pragma once

#include "common.hpp"

class SpriteSheet
{

public:

    void init(Texture* texture, const vec2 spriteDims, Renderable* renderable);

    bool set_render_data(Renderable *renderable, int index);

    void update_render_data(Renderable *renderable, int index);

    Texture* texture;
    vec2 dims;

};