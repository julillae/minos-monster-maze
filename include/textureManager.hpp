#pragma once

#include "common.hpp"

class TextureManager
{

public:

    bool static load_texture(const char* fileName, Texture* texture, Renderable* renderable);

};
