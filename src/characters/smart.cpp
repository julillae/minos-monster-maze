#include "../include/characters/smart.hpp"

// Put shared implementation for smart enemies here

Texture Smart::smart_texture;

bool Smart::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;
    
    const char* textureFile = textures_path("minotaur_right.png");
	if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    RenderManager::set_render_data(&m_texture, this);

	set_properties(initialPosition, 0.4f, speed);
	width = smart_texture.width * m_scale.x;
	height = smart_texture.height * m_scale.y;
	direction = right;
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