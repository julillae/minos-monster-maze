#pragma once

#include "common.hpp"

class RenderManager
{

public:

    bool static load_texture(const char* fileName, Texture* texture, Renderable* renderable);

	bool static set_vertex_data(Texture * texture, Renderable * renderable);

    bool static set_render_data(Texture* texture, Renderable* renderable);

    void static draw_texture(const mat3& projection, vec2 position, float rotation, vec2 scale, Texture* texture, Renderable* renderable);

    void static draw_mesh(const mat3& projection, Renderable* renderable, float* color, size_t num_indices);

    void static init_drawing_data(vec2 position, float rotation, vec2 scale, Renderable* renderable);

    void static draw_texture_color(const mat3& projection, vec2 position, float rotation, vec2 scale, float* color,
                                   Texture* texture, Renderable* renderable);

    void static draw_hide_texture(const mat3& projection, vec2 position, float rotation, vec2 scale, Texture* texture,
								  bool is_hidden, Renderable* renderable);

};
