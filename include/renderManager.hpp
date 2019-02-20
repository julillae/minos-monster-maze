#pragma once

#include "common.hpp"

class RenderManager
{

public:

    bool static load_texture(const char* fileName, Texture* texture, Renderable* renderable);

    bool static set_render_data(Texture* texture, Renderable* renderable);

    void static draw(const mat3& projection, vec2 position, float rotation, vec2 scale, Texture* texture, Renderable* renderable);

};
