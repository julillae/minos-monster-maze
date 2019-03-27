#include <stack>

#include "../include/characters/minotaur.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"
#include "../include/gameAI.hpp"

// Put implementation for Minotaur enemy here

Texture Minotaur::minotaur_texture;

bool Minotaur::init(vec2 initialPosition, Physics * physicsHandler)
{
    Smart::init(initialPosition, physicsHandler);

    const char* textureFile = textures_path("bat-sprite-sheet.png");
	if (!RenderManager::load_texture(textureFile, &minotaur_texture, this)) {
        return false;
    }

	float spriteSheetWidth = 5.0f;
	float spriteSheetHeight = 3.0f;
    int horizontalTrim = 12;
    int verticalTrim = 19;
    m_stopBound = 10.f;

	spriteSheet.init(&minotaur_texture, { spriteSheetWidth, spriteSheetHeight }, this);

	spriteSheet.set_render_data(this, 0);

	initStateTree();
	set_properties(initialPosition, 3.0f, speed);
	m_frozen = false;
	set_dimensions(&minotaur_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);
	characterState->changeState(idle);

	return true;
}

void Minotaur::update(float ms)
{
	if (is_alive() && !m_frozen)
	{
        // Want to move in direction of player if within a certain distance. Will stop at stop_bound if player still within range, be idle
        // Also want to be able to rotate world?
        
        // Update enemy position based on fixed path here
		float vel_x = m_velocity.x;
		float vel_y = m_velocity.y;
		float right_bound = m_initialPosition.x + m_stopBound;
		float left_bound = m_initialPosition.x;
		float next_pos_x = m_position.x + vel_x;

		if (((vel_x > 0) && (next_pos_x >= right_bound)) ||
			((vel_x < 0) && (next_pos_x <= left_bound))) {
			vel_x *= -1;
			m_scale.x *= -1;
			set_velocity({ vel_x, vel_y });
		}
		move();
	}

}

void Minotaur::set_bound(float bound) 
{
   m_stopBound = bound;
}

void Minotaur::draw(const mat3& projection)
{
    set_animation();
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &minotaur_texture, this);

}

void Minotaur::set_animation()
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
            case running:
            case rising:
            case falling:
                numTiles = 5;
                tileIndex = 0;
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
            tileIndex = 14;
        } else
        {
            numTiles = 5;
            tileIndex = 10;
        }
    }

    // Increment animation time
    m_animTime += animSpeed;

    // Apply animation
    tileIndex = tileIndex + (int)m_animTime % numTiles;

    // do not repeat death animation
    if (!isRepeat && tileIndex == 14) is_anim_once = true;

    spriteSheet.update_render_data(this, tileIndex);
}