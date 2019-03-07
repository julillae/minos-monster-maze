#pragma once

#include <stack>
#include <chrono>

// Simple interface for the harpy enemies. These are 'smart' enemies that use AI to follow the player
// harpies can fly, so they should be able to follow the player in any direction (without worrying about
// the physics of jumping).
#include "enemy.hpp"
#include "character.hpp"
#include "smart.hpp"

class Harpy: public Smart
{
    static Texture harpy_texture;
    public:
        // Creates all the associated render resources and default transform
        bool init(vec2 initialPosition, Physics* physicsHandler)override;

        // Update player position based on velocity vector
        // ms represents the number of milliseconds elapsed from the previous update() call
        void update(float ms)override;

        void draw(const mat3& projection)override;

        void set_animation()override;

        bool checkTimeElapsed();

        void resetCycleStart();

        void moveAlongPath();

        void updateVelocity();
    private:
        std::stack<vec2> path_to_follow;
        std::chrono::_V2::system_clock::time_point cycle_start;
        const float path_cycle_time = 100.f;
        vec2 next_node;
        float maxVerticalSpeed = 2.f;
        float maxHorzSpeed = 2.f;
	
};