#pragma once

// Simple interface for all enemies without AI behaviour
// ie. the enemies that just patroll back and forth on predetermined path relative to map rotation
// Put shared methods for such enemies here
// Make sure simple enemies implement both this interface and the Enemy superclass
#include "enemy.hpp"

class Simple: public Enemy
{
    static Texture enemy_texture;
    public:
        bool set_texture(const char* texturePath);
        void update(float ms)override;
        void draw(const mat3& projection)override;
        bool init(vec2 initialPosition);
        void set_bound(float bound);
        // gets bounding box of enemy
	    vec2 get_bounding_box()const;
        // Sets the enemy position to its initial position
	    void reset_position();
    private:
        double tolerance = 0.000001;
        float maxVelocity = 2.0f;
        vec2 currentVelocity;
        Direction direction;
        float stopBound;
        vec2 m_initialPosition;
        bool m_frozen;
};