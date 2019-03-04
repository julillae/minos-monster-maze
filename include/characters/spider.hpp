#pragma once

// Simple interface for the spider enemies. These are 'simple' enemies that patrol back and forth
#include "enemy.hpp"
#include "character.hpp"
#include "simple.hpp"

class Spider: public Simple
{
    static Texture spider_texture;
    static Texture b_texture;
    public:
        // Creates all the associated render resources and default transform
        bool init(vec2 initialPosition, Physics* physicsHandler)override;

        void draw(const mat3& projection)override;

        void set_animation()override;
	
};