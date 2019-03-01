#include <stack>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

#include "../include/characters/harpy.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put implementation for Harpy enemies here

Texture Harpy::harpy_texture;

bool Harpy::init(vec2 initialPosition, Physics * physicsHandler)
{
	Smart::init(initialPosition, physicsHandler);

    const char* textureFile = textures_path("spider-sprite-sheet.png");
	if (!RenderManager::load_texture(textureFile, &harpy_texture, this)) return false;

	float spriteSheetWidth = 9.0f;
	float spriteSheetHeight = 3.0f;
    int horizontalTrim = 16;
    int verticalTrim = 19;

	spriteSheet.init(&harpy_texture, { spriteSheetWidth, spriteSheetHeight }, this);

	spriteSheet.set_render_data(this, 0);

	initStateTree();
	set_properties(initialPosition, 3.0f, speed);
	m_frozen = false;
	set_dimensions(&harpy_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);
	characterState->changeState(idle);

    path_to_follow = stack<vec2>();
    cycle_start = Clock::now();

	return true;
}

void Harpy::update(float ms)
{
	if (is_alive() && !m_frozen)
	{
        bool update_path = checkTimeElapsed();
        if (path_to_follow.empty() || update_path)
        {
         // call AI to get a path to the player   
        } else {
            moveAlongPath();
	}

}

void Harpy::moveAlongPath(){
    // pop off the next node to visit in the path
}

bool Harpy::checkTimeElapsed() {
    auto now = Clock::now();
	float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - cycle_start)).count() / 1000;
    if (elapsed_sec >= path_cycle_time) {
        resetCycleStart();
        return true;
    }
    return false;
}

void Harpy::resetCycleStart() {
    cycle_start = Clock::now();
}

void Harpy::draw(const mat3& projection)
{
    set_animation();
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &harpy_texture, this);

}

void Harpy::set_animation()
{
    int numTiles;
    int tileIndex;
    bool isRepeat = true;

    float animSpeed = 0.2f;

    if (is_alive())
    {
        is_anim_once = false;
        switch (characterState->currentState) {
            case idle:
                numTiles = 5;
                tileIndex = 0;
                break;
            case running:
                numTiles = 6;
                tileIndex = 5;
                break;
            default:
                numTiles = 1;
                tileIndex = 0;

        }
    } else {
        isRepeat = false;

        if (is_anim_once)
        {
            numTiles = 1;
            tileIndex = 19;
        } else
        {
            numTiles = 9;
            tileIndex = 11;
        }
    }

    // Increment animation time
    m_animTime += animSpeed;

    // Apply animation
    tileIndex = tileIndex + (int)m_animTime % numTiles;

    // do not repeat death animation
    if (!isRepeat && tileIndex == 19) is_anim_once = true;

    spriteSheet.update_render_data(this, tileIndex);
}