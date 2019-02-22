#pragma once

// Smart interface for all enemies with AI behaviour
// Put shared methods for such enemies here
// Make sure smart enemies implement both this interface and the Enemy superclass
#include "enemy.hpp"
#include "character.hpp"

class Smart: public Enemy
{
    static Texture smart_texture;
    public:
        // Creates all the associated render resources and default transform
        bool init(vec2 initialPosition, Physics* physicsHandler)override;
        
        // Update player position based on velocity vector
        // ms represents the number of milliseconds elapsed from the previous update() call
        void update(float ms)override;

};