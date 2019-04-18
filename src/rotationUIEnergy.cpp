#include "../include/rotationUIEnergy.hpp"

bool RotationUIEnergy::init()
{
    m_texture.id = 0;

    const char* textureFile = textures_path("rotation-energy.png");

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    if (!RenderManager::set_render_data(&m_texture, this)) return false;

    if (!effect.load_from_file(shader_path("visibility.vs.glsl"), shader_path("visibility.fs.glsl")))
        return false;

    m_rotation = 0.f;
    m_scale = {1.25f, 1.25f};
    set_visibility(true);

    return true;
}

void RotationUIEnergy::draw(const mat3& projection)
{
    RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &m_texture, color, is_hidden, this);
}

void RotationUIEnergy::update(float energyLevel)
{

    // The position corresponds to the center of the texture
    float wr = m_texture.width * 0.5f * energyLevel;
    float hr = m_texture.height * 0.5f;

    TexturedVertex vertices[4];

    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 0.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { energyLevel, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { energyLevel, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 0.f, 0.f };

    // Clearing errors
    gl_flush_errors();

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);
}

void RotationUIEnergy::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;
}

void RotationUIEnergy::set_position(vec2 pos) {

    float original_leftEdge = pos.x - m_texture.width * 0.5f * m_scale.x;

    float wr = m_texture.width * 0.5f * energyLevel;

    // adjust position due to smaller texture
    float leftEdge = pos.x - wr * m_scale.x;
    float posDiff = leftEdge - original_leftEdge;
    m_position = vec2({pos.x - posDiff, pos.y});
}

void RotationUIEnergy::set_energy_level(float energyLevel)
{
    this->energyLevel = energyLevel;
}

float RotationUIEnergy::get_energy_level()
{
    return energyLevel;
}