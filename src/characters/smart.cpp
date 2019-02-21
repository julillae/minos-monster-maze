#include "../include/characters/smart.hpp"

// Put shared implementation for smart enemies here

Texture Smart::smart_texture;

bool Smart::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;
    
    const char* textureFile = textures_path("minotaur_right.png");
	if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    RenderManager::set_render_data(&m_texture, this);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	float scaleFactor = 0.4f;
	m_scale.x = scaleFactor;
	m_scale.y = scaleFactor;
	width = m_texture.width * scaleFactor;
	height = m_texture.height * scaleFactor;
	m_rotation = 0.f;
	direction = right;
	m_is_alive = true;
	m_position = initialPosition;
	m_velocity = {speed, 0.0f};
	m_initialPosition = initialPosition;

	return true;
}

void Smart::update(float ms)
{

	if (m_is_alive)
	{
		// Update enemy position to follow the player using enemy AI
		
	}
	else
	{
		// If dead reset do something
	}

}