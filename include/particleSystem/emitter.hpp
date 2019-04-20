//
// Created by Henry Deng on 2019-04-18.
//

#pragma once

#include "../include/particleSystem/particle.hpp"

class Emitter: public Renderable {

public:
    Emitter(vec2 position, int max_particles);

    ~Emitter();

    void update(float ms);

    bool init();
    void draw(const mat3& projection);
    int get_alive_particles();

private:
    // particles attached to the emitter
    Particle * attached_to;
    int m_max_particles;
    int m_num_alive_particles;
    vec2 m_position;
};

