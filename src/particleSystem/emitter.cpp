//
// Created by Henry Deng on 2019-04-18.
//

#include "../../include/particleSystem/emitter.hpp"
#include <stdlib.h>
#include <vector>

Emitter::Emitter(vec2 position, int max_particles) {
    m_max_particles = max_particles;
    m_num_alive_particles = 0;
    attached_to = new Particle[max_particles];
    m_position = position;
}

Emitter::~Emitter() {
    delete[] attached_to;
}

void Emitter::update(float ms) {
    int i;
    for (i = 0; i < m_num_alive_particles; i++) {
        attached_to[i].update(ms);
        if (!attached_to[i].is_particle_alive()) {
            attached_to[i] = attached_to[--m_num_alive_particles];

        }
    }
}

static const Vertex vertex_buffer_data[] = {
        { { -0.5f, -0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
        { { 0.5f, -0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
        { { -0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
        { { 0.5f,  0.5f, -0.01f },{ 1.f, 0.f, 0.f } },
};

bool Emitter::init()
{

    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertex_buffer_data, GL_STATIC_DRAW);

    // Loading shaders
    effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl"));

    int i = 0;
    while (i < m_max_particles) {
        // initialize particles here
        attached_to[i++].init(m_position, 80, (rand() % 30000) / 100.f, (rand() % 1000) / 1000.f + 2.f);
        m_num_alive_particles++;
    }

    if (gl_has_errors()) {
        return false;
    }

    return true;
}

void Emitter::draw(const mat3& projection) {
    if (m_num_alive_particles > 0) {

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

        // Setting shaders
        glUseProgram(effect.program);

        for (int i = 0; i < m_num_alive_particles; i++) {
            attached_to[i].draw(projection, effect);
        }
    }
}

int Emitter::get_alive_particles() {
    return m_num_alive_particles;
}