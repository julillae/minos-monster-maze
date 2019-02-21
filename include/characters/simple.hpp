#pragma once

// Simple interface for all enemies without AI behaviour
// ie. the enemies that just patroll back and forth on predetermined path relative to map rotation
// Put shared methods for such enemies here
// Make sure simple enemies implement both this interface and the Enemy superclass
#include "enemy.hpp"
#include "character.hpp"

class Simple: public Enemy
{
    static Texture simple_texture;
    public:
        // Creates all the associated render resources and default transform
        bool init(vec2 initialPosition, Physics* physicsHandler)override;
        
        // Update player position based on velocity vector
        // ms represents the number of milliseconds elapsed from the previous update() call
        void update(float ms)override;

        void set_bound(float bound);

        void draw(const mat3& projection)override;
    // protected:
        float m_stopBound;
	
};