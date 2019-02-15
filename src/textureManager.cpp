#include "../include/textureManager.hpp"

bool TextureManager::load_texture(const char* fileName, Texture* texture, Renderable* renderable)
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

    // The position corresponds to the center of the texture
    float wr = texture->width * 0.5f;
    float hr = texture->height * 0.5f;

    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.01f };
    vertices[0].texcoord = { 0.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.01f };
    vertices[1].texcoord = { 1.f, 1.f,  };
    vertices[2].position = { +wr, -hr, -0.01f };
    vertices[2].texcoord = { 1.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.01f };
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

    // Loading shaders
    if (!renderable->effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;

    return true;

}

