#pragma once

#include "common.hpp"

class TextureManager
{

public:

    bool static load_texture(const char* fileName, Texture* texture, Renderable* renderable);

    bool static set_render_data(Texture* texture, Renderable* renderable);

};
