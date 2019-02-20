// Header
#include "../include/characters/player.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <cmath>

//Texture Player::player_texture;

bool Player::init(vec2 initialPosition)
{

	const char* textureFile = textures_path("player-sprite-sheet.png");

	if (!TextureManager::load_texture(textureFile, &m_texture, this)) return false;

	float spriteSheetWidth = 8.0f;
	float spriteSheetHeight = 5.0f;

	spriteSheet.init(&m_texture, { spriteSheetWidth, spriteSheetHeight });

	spriteSheet.set_render_data(this, 0);
	
	// Setting initial values
    float scaleFactor = 2.0f;
	m_scale.x = -scaleFactor;
    m_scale.y = scaleFactor;
    width = m_texture.width / spriteSheetWidth * scaleFactor - 12;
    height = m_texture.height / spriteSheetHeight * scaleFactor - 14;
	m_is_alive = true;
	v_direction = Direction::none;
	m_position = initialPosition;
	m_rotation = 0.f;
	currentVelocity = {0.0, 0.0};

	isBelowPlatform = false;
	isLeftOfPlatform = false;
	isRightOfPlatform = false;

	return true;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	float vAcc;
	float hAcc;

	if (m_is_alive)
	{
		switch (h_direction) {
			case Direction::left: hAcc = -accStep; break;
			case Direction::right: hAcc = accStep; break;
			default: hAcc = 0.f; break;
		}
		vAcc = gravityAcc;

		set_acceleration({ hAcc, vAcc });
		update_velocity();
		move();

	}

}

void Player::draw(const mat3& projection)
{
	set_animation();

	transform_begin();

	transform_translate(m_position);
	transform_scale(m_scale);
	transform_rotate(m_rotation);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// Player Color
	float color[] = { 1.f, 1.f, 1.f };

	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// magnifies texture to avoid it being blurry when scaled
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Drawing!
	glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_SHORT, nullptr);
}

// Returns the local bounding coordinates scaled by the current size of the player 
vec2 Player::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { width, height };
}

void Player::set_acceleration(vec2 acc)
{
	currentAcceleration.x = acc.x; currentAcceleration.y = acc.y;
}

void Player::update_velocity()
{
	currentVelocity.x += currentAcceleration.x;
	currentVelocity.y += currentAcceleration.y;

	if (currentVelocity.x > maxVelocity) currentVelocity.x = maxVelocity;
	if (currentVelocity.x < -maxVelocity) currentVelocity.x = -maxVelocity;

	if (currentAcceleration.x < tolerance && currentAcceleration.x > -tolerance && isOnPlatform)
		currentVelocity.x *= drag;

	if (isBelowPlatform) {
		currentVelocity.y = std::max(0.f, currentVelocity.y);
	}
	if (isLeftOfPlatform) {
		currentVelocity.x = std::min(0.f, currentVelocity.x);
	}
	if (isRightOfPlatform) {
		currentVelocity.x = std::max(0.f, currentVelocity.x);
	}
}

void Player::move()
{
	m_position.x += currentVelocity.x; m_position.y += currentVelocity.y;
	currentFloorPos = std::min(fakeFloorPos, currentFloorPos);
	if (m_position.y >= currentFloorPos - tolerance) {
		m_position.y = currentFloorPos;
		currentVelocity.y = 0.f;
		isOnPlatform = true;
	}
	else {
		set_in_free_fall();
	}

}

void Player::set_on_platform(float yPos) {
	isOnPlatform = true;
	currentFloorPos = yPos;
}

void Player::set_in_free_fall() {
	isOnPlatform = false;
	currentFloorPos = fakeFloorPos;
}

void Player::set_direction(int key, int action)
{
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_UP: if (isOnPlatform) currentVelocity.y += jumpVel; break;
			case GLFW_KEY_LEFT: 
				h_direction = Direction::left; 
				m_scale.x = -std::fabs(m_scale.x);
				break;
			case GLFW_KEY_RIGHT: 
				h_direction = Direction::right; 
				m_scale.x = std::fabs(m_scale.x);
				break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_UP: 
				if (v_direction == Direction::up)
					v_direction = Direction::none; break;
			case GLFW_KEY_LEFT: 
				if (h_direction == Direction::left)
					h_direction = Direction::none; break;
			case GLFW_KEY_RIGHT: 
				if (h_direction == Direction::right)
					h_direction = Direction::none; break;
		}
	}
}


void Player::set_rotation(float radians)
{
	m_rotation = radians;
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Player::kill()
{
	m_is_alive = false;
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
		if (currentAcceleration.x == 0.f)
		{
			numTiles = 5;
			tileIndex = 0;
		}

		// running animation
		if (currentAcceleration.x != 0.f)
		{
			numTiles = 8;
			tileIndex = 8;
		}

		// jump up
		if (currentVelocity.y < 0)
		{
			numTiles = 1;
			tileIndex = 9;
		}

		// falling down
		if (currentVelocity.y > 0)
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

	spriteSheet.set_render_data(this, tileIndex);
}

