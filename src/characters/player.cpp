// Header
#include "../include/characters/player.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <cmath>

bool Player::init(vec2 initialPosition, Physics* physicsHandler)
{
	this->physicsHandler = physicsHandler;

	const char* textureFile = textures_path("player-sprite-sheet.png");

	if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	float spriteSheetWidth = 8.0f;
	float spriteSheetHeight = 5.0f;

	spriteSheet.init(&m_texture, { spriteSheetWidth, spriteSheetHeight }, this);

	spriteSheet.set_render_data(this, 0);
	
	// Setting initial values
    float scaleFactor = 2.0f;
	m_scale.x = -scaleFactor;
    m_scale.y = scaleFactor;
	int horizontalTrim = 12;
	int verticalTrim = 14;
    width = m_texture.width / spriteSheetWidth * scaleFactor - horizontalTrim;
    height = m_texture.height / spriteSheetHeight * scaleFactor - verticalTrim;
	m_is_alive = true;
	m_position = initialPosition;
	m_rotation = 0.f;
	m_velocity = {0.0, 0.0};

	isBelowPlatform = false;
	isLeftOfPlatform = false;
	isRightOfPlatform = false;

	return true;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	physicsHandler->characterAccelerationUpdate(this);
	physicsHandler->characterVelocityUpdate(this);
	if (m_is_alive)	move();
}

void Player::draw(const mat3& projection)
{
	set_animation();

	RenderManager::draw(projection, m_position, m_rotation, m_scale, &m_texture, this);
}

// Returns the local bounding coordinates scaled by the current size of the player 
vec2 Player::get_bounding_box()const {
	// fabs is to avoid negative scale due to the facing direction
	return { width, height };
}

void Player::set_on_platform() {
	isOnPlatform = true;
}

void Player::set_in_free_fall() {
	isOnPlatform = false;
}

void Player::on_key(int key, int action)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_UP: if (isOnPlatform) m_velocity.y += jumpVel; break;
		case GLFW_KEY_LEFT:
			direction = Direction::left;
			m_scale.x = -std::fabs(m_scale.x);
			break;
		case GLFW_KEY_RIGHT:
			direction = Direction::right;
			m_scale.x = std::fabs(m_scale.x);
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_LEFT:
			if (direction == Direction::left)
				direction = Direction::none; break;
		case GLFW_KEY_RIGHT:
			if (direction == Direction::right)
				direction = Direction::none; break;
		}
	}
}

void Player::set_animation()
{
	int numTiles;
	int tileIndex;
	bool isRepeat = true;

	float animSpeed = 0.2f;

	// Calculate animation
	if (m_is_alive)
	{
		is_anim_once = false;

		// idle animation
		if (m_acceleration.x == 0.f)
		{
			numTiles = 5;
			tileIndex = 0;
		}

		// running animation
		if (m_acceleration.x != 0.f)
		{
			numTiles = 8;
			tileIndex = 8;
		}

		// jump up
		if (m_velocity.y < 0)
		{
			numTiles = 1;
			tileIndex = 9;
		}

		// falling down
		if (m_velocity.y > 0)
		{
			numTiles = 1;
			tileIndex = 14;
		}

	} else
	{
		isRepeat = false;

		if (is_anim_once)
		{
			numTiles = 1;
			tileIndex = 38;
		} else
		{
			numTiles = 7;
			tileIndex = 32;
		}

	}

	// Increment animation time
	m_animTime += animSpeed;

	// Apply animation
	tileIndex = tileIndex + (int)m_animTime % numTiles;

	// do not repeat death animation
	if (!isRepeat && tileIndex == 38) is_anim_once = true;

	spriteSheet.update_render_data(this, tileIndex);
}