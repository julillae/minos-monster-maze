// Header
#include "../include/characters/player.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"
#include "../include/level.hpp"

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
	const char* textureFile;

	// if use_sprite set to true, uses player sprite
    // else uses coloured box representing size of bounding box
	use_sprite = true;

	textureFile = textures_path("player-sprite-sheet.png");
	if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	float spriteSheetWidth = 8.0f;
	float spriteSheetHeight = 5.0f;
	int horizontalTrim = 7;
	int verticalTrim = 7;
	set_properties(initialPosition, 2.0f, 0.f);
	set_dimensions(&m_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);

	if (use_sprite)
	{
		spriteSheet.init(&m_texture, { spriteSheetWidth, spriteSheetHeight }, this);
		spriteSheet.set_render_data(this, 0);

	} else
	{
		textureFile = textures_path("solid.png");
		if (!RenderManager::load_texture(textureFile, &box_texture, this)) return false;
		if (!RenderManager::set_render_data(&box_texture, this)) return false;

		m_scale.x = width / box_texture.width;
		m_scale.y = height / box_texture.height;

	}

    initStateTree();

	// kept in for debugging
//	fprintf(stderr, "player height: %f\n", height); // 50.00f
//	fprintf(stderr, "player width: %f\n", width); // 52.00f

	isBelowPlatform = false;

	return true;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	physicsHandler->characterAccelerationUpdate(this);
	physicsHandler->characterVelocityUpdate(this);
	if (is_alive()) move();
	collisionNormals.clear();
}

void Player::draw(const mat3& projection)
{
	if (use_sprite)
	{
		set_animation();
		RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &m_texture, this);
	} else
	{
		RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &box_texture, this);

	}

}

// Returns the local bounding coordinates scaled by the current size of the player 
vec2 Player::get_bounding_box()const {
	// fabs is to avoid negative scale due to the facing direction
	return { width, height };
}

void Player::set_on_platform() {
	characterState->changeState(landing);
	isOnPlatform = true;
}

void Player::set_in_free_fall() {
	isOnPlatform = false;
}

void Player::on_key(int key, int action)
{
	if (action == GLFW_PRESS) {
		if (key == jumpKey) {
			if (can_jump()) characterState->changeState(jumping);
		}
		else if (key == GLFW_KEY_LEFT) {
			direction = Direction::left;
			m_scale.x = -std::fabs(m_scale.x);
		}
		else if (key == GLFW_KEY_RIGHT) {
			direction = Direction::right;
			m_scale.x = std::fabs(m_scale.x);
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_LEFT:
			if (direction == Direction::left) {
				direction = Direction::none;
			}
			break;
		case GLFW_KEY_RIGHT:
			if (direction == Direction::right) {
				direction = Direction::none;
			}
			break;
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
	if (is_alive())
	{
		is_anim_once = false;
		switch (characterState->currentState) {
		case idle:
			numTiles = 5;
			tileIndex = 0;
			break;
		case running:
			numTiles = 8;
			tileIndex = 8;
			break;
		case jumping:
		case rising:
			numTiles = 1;
			tileIndex = 9;
			break;
		case falling:
			numTiles = 1;
			tileIndex = 14;
		default:
			numTiles = 1;
			tileIndex = 0;
		}
	} else {
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

bool Player::can_jump()
{
	return characterState->getStateChangeCost(jumping).first;
}

std::vector<vec2> Player::getCollisionNormals() {
	return collisionNormals;
}