#include "../include/spriteSheet.hpp"

void SpriteSheet::init(Texture* texture, const vec2 spriteDims)
{
    this->texture = texture;
    this->dims = spriteDims;
}

bool SpriteSheet::set_render_data(Renderable* renderable, int index)
{
    float spriteWidth = texture->width / dims.x;
    float spriteHeight = texture->height / dims.y;
    float xStep =  1 / dims.x;
    float yStep =  1 / dims.y;

    // The position corresponds to the center of the texture
    float wr = spriteWidth * 0.5f;
    float hr = spriteHeight * 0.5f;

    int xTile = index % (int) dims.x;
    int yTile = (int) (index / (int) dims.x) % (int) dims.y;

    TexturedVertex vertices[4];

    vertices[0].position = { -wr, +hr, -0.01f };
    vertices[0].texcoord = { xStep * xTile, yStep * (yTile + 1) };
    vertices[1].position = { +wr, +hr, -0.01f };
    vertices[1].texcoord = { xStep * (xTile + 1), yStep * (yTile + 1) };
    vertices[2].position = { +wr, -hr, -0.01f };
    vertices[2].texcoord = { xStep * (xTile + 1), yStep * yTile };
    vertices[3].position = { -wr, -hr, -0.01f };
    vertices[3].texcoord = { xStep * xTile, yStep * yTile };

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

