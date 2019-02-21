#include "../include/characters/spider.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put implementation for Spider enemies here

Texture Spider::spider_texture;

bool Spider::init(vec2 initialPosition, Physics * physicsHandler)
{
	Simple::init(initialPosition, physicsHandler);

    // TODO: use a spider texture here rather than the minotaur
    const char* textureFile = textures_path("minotaur_right.png");
	if (!RenderManager::load_texture(textureFile, &spider_texture, this)) return false;

	RenderManager::set_render_data(&spider_texture, this);

	set_properties(initialPosition, 0.4f, speed);
	width = spider_texture.width * m_scale.x;
	height = spider_texture.height * m_scale.y;

	return true;
}

void Spider::draw(const mat3& projection)
{
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &spider_texture, this);

}