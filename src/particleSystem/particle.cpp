//
// Created by Henry Deng on 2019-04-10.
//

#include "../../include/particleSystem/particle.hpp"
#include <math.h>

Particle::Particle() {
    m_position = { 0, 0};
    m_lifespan = 0;
    m_velocity = { 0, 0};
    is_alive = true;
}

void Particle::init(vec2 position, float lifespan, float angle, float speed)
{
    float rads = fmod(angle * 3.14f, 180.f);
    m_position = position;
    m_lifespan = lifespan;
    m_velocity.x = speed * cos(rads);
    m_velocity.y = -speed * sin(rads);
    m_size = 8.f;
}

void Particle::update(float elapsed_ms) {
    m_lifespan -= elapsed_ms;
    if (m_lifespan < 0) {
        is_alive = false;
        return;
    }

    // perform some transformations
    float transform_x = -m_velocity.x * 0.5f * 1.12f * 0.35f * 0.05f * pow(m_velocity.x, 2);
    float transform_y = -m_velocity.y * 0.5f * 1.12f * 0.35f * 0.05f * pow(m_velocity.y, 2);

    // update velocity
    m_velocity.x += transform_x * (elapsed_ms / 1000);
    m_velocity.y += transform_y * (elapsed_ms / 1000);

    // update position of particles
    m_position.x += m_velocity.x;
    m_position.y += m_velocity.y;
}

bool Particle::is_particle_alive() {
    return is_alive;
}

void Particle::draw(const mat3& projection) {
    // override function
    }

void Particle::draw(const mat3& projection, Effect& effect) {
    transform_begin();
    transform_translate(m_position);
    transform_scale({ m_size, m_size });


    transform_end();

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

    // Getting uniform locations
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
}




