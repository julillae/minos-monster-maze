#include "../include/characters/spider.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put implementation for Spider enemies here

Texture Spider::spider_texture;

bool Spider::init(vec2 initialPosition, Physics * physicsHandler)
{
	this->physicsHandler = physicsHandler;

    // TODO: use a spider texture here rather than the minotaur
    const char* textureFile = textures_path("minotaur_right.png");
	if (!RenderManager::load_texture(textureFile, &spider_texture, this)) return false;

	 RenderManager::set_render_data(&spider_texture, this);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	float scaleFactor = 0.4f;
	m_scale.x = scaleFactor;
	m_scale.y = scaleFactor;
	width = spider_texture.width * scaleFactor;
	height = spider_texture.height * scaleFactor;
	m_rotation = 0.f;
	direction = right;
	m_is_alive = true;
	m_position = initialPosition;
	m_velocity = {speed, 0.0f};
	m_stopBound = 10.f;
	m_initialPosition = initialPosition;

	return true;
}

void Spider::draw(const mat3& projection)
{
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &spider_texture, this);

}