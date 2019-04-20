//
// Created by Henry Deng on 2019-04-10.
//
#pragma once

#include "../include/common.hpp"

class Particle: public Renderable {

public:
    Particle();
    void init (vec2 position, float lifespan, float angle, float speed);
    void update (float ms);

    void draw(const mat3& projection) override;
    void draw(const mat3& projection, Effect& emitter_effect);

    bool is_particle_alive();

private:
    vec2 m_position{};
    bool is_alive;
    vec2 m_velocity{};
    float m_lifespan;
    float m_size{};







};


