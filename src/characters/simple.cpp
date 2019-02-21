#include "../include/characters/simple.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put shared implementation for simple enemies here

Texture Simple::simple_texture;

bool Simple::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;
    
    const char* textureFile = textures_path("minotaur_right.png");
	if (!RenderManager::load_texture(textureFile, &simple_texture, this)) return false;

	 RenderManager::set_render_data(&simple_texture, this);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	float scaleFactor = 0.4f;
	m_scale.x = scaleFactor;
	m_scale.y = scaleFactor;
	width = simple_texture.width * scaleFactor;
	height = simple_texture.height * scaleFactor;
	m_rotation = 0.f;
	direction = right;
	m_is_alive = true;
	m_position = initialPosition;
	m_velocity = {speed, 0.0f};
	m_stopBound = 10.f;
	m_initialPosition = initialPosition;

	return true;
}

void Simple::update(float ms)
{

	if (m_is_alive)
	{
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
	else
	{
		// If dead reset do something
	}

}

void Simple::draw(const mat3& projection)
{
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &simple_texture, this);

}

void Simple::set_bound(float bound) 
{
   m_stopBound = bound;
}