#include "../include/renderManager.hpp"

bool RenderManager::load_texture(const char* fileName, Texture* texture, Renderable* renderable)
{

    // Load shared texture
    if (!texture->is_valid())
    {
        if (!texture->load_from_file(fileName))
        {
            fprintf(stderr, "Failed to load texture from %s!", fileName);
            return false;
        }
    }

    return true;

}

bool RenderManager::set_render_data(Texture *texture, Renderable *renderable)
{
    // The position corresponds to the center of the texture
    float wr = texture->width * 0.5f;
    float hr = texture->height * 0.5f;

    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 0.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 1.f, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 1.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 0.f, 0.f };

    // counterclockwise as it's the default opengl front winding direction
    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &renderable->mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderable->mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &renderable->mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &renderable->mesh.vao);
    if (gl_has_errors())
        return false;

    return true;
}

void RenderManager::draw_texture(const mat3& projection, vec2 position, float rotation, vec2 scale, Texture* texture, Renderable* renderable)
{

    init_drawing_data(position, rotation, scale, renderable);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(renderable->effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(renderable->effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(renderable->effect.program, "projection");

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(renderable->effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(renderable->effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&renderable->transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // magnifies texture to avoid it being blurry when scaled
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void RenderManager::draw_mesh(const mat3& projection, vec2 position, float rotation, vec2 scale, Renderable* renderable,
                              float* color, size_t num_indices)
{

    init_drawing_data(position, rotation, scale, renderable);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(renderable->effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(renderable->effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(renderable->effect.program, "projection");

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(renderable->effect.program, "in_position");
    GLint in_color_loc = glGetAttribLocation(renderable->effect.program, "in_color");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&renderable->transform);
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES,(GLsizei)num_indices, GL_UNSIGNED_SHORT, nullptr);
}

void RenderManager::init_drawing_data(vec2 position, float rotation, vec2 scale, Renderable* renderable)
{
    renderable->transform_begin();
    renderable->transform_translate(position);
    renderable->transform_rotate(rotation);
    renderable->transform_scale(scale);
    renderable->transform_end();

    // Setting shaders
    glUseProgram(renderable->effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Setting vertices and indices
    glBindVertexArray(renderable->mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderable->mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->mesh.ibo);
}

