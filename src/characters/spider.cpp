#include "../include/characters/spider.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

// Put implementation for Spider enemies here

Texture Spider::spider_texture;
Texture Spider::b_texture;

bool Spider::init(vec2 initialPosition, Physics * physicsHandler)
{
	Simple::init(initialPosition, physicsHandler);

    const char* textureFile;

    // if use_sprite set to true, uses player sprite
    // else uses coloured box representing size of bounding box
    use_sprite = true;

    textureFile = textures_path("spider-sprite-sheet.png");
    if (!RenderManager::load_texture(textureFile, &spider_texture, this)) return false;
    
    float spriteSheetWidth = 9.0f;
    float spriteSheetHeight = 3.0f;
    int horizontalTrim = 30;
    int verticalTrim = 20;

    set_properties(initialPosition, 3.0f, speed);
    set_dimensions(&spider_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);

    if (use_sprite)
    {

        spriteSheet.init(&spider_texture, { spriteSheetWidth, spriteSheetHeight }, this);
        spriteSheet.set_render_data(this, 0);

    } else
    {
        textureFile = textures_path("blue.png");
        if (!RenderManager::load_texture(textureFile, &b_texture, this)) return false;
        if (!RenderManager::set_render_data(&b_texture, this)) return false;

        m_scale.x = width / b_texture.width;
        m_scale.y = height / b_texture.height;

    }

	initStateTree();

	m_frozen = false;
	characterState->changeState(running);

	return true;
}

void Spider::draw(const mat3& projection)
{
    if (use_sprite)
    {
        set_animation();
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &spider_texture, color, is_hidden, this);

    } else
    {
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &b_texture, color, is_hidden, this);

    }

}

void Spider::set_animation()
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

bool Spiders::spawn_spider_enemy(vec2 position, float bound, bool upsideDown)
{
	Spider enemy;

	if (enemy.init(position, physicsHandler))
	{
		if (upsideDown) {
			enemy.set_rotation(M_PI);
			vec2 enemy_scale = enemy.get_scale();
			enemy.set_scale({ enemy_scale.x * -1.f, enemy_scale.y });
		}

		enemy.set_bound(bound);
		m_spiders.emplace_back(enemy);

		return true;
	}
	fprintf(stderr, "Failed to spawn enemy");
	return false;
}

std::vector<Spider> Spiders::get_spider_vector()
{
	return m_spiders;
}

void Spiders::draw(const mat3 & projection)
{
	for (auto& spider : m_spiders)
		spider.draw(projection);
}

void Spiders::reset()
{
	for (auto& spider : m_spiders) {
		spider.freeze();
		spider.reset_position();
		spider.unfreeze();
	};
}

void Spiders::freeze()
{
	for (auto& s : m_spiders) s.freeze();
}

void Spiders::unfreeze()
{
	for (auto& s : m_spiders) s.unfreeze();
}

void Spiders::update(float elapsed_ms)
{
	for (auto& s : m_spiders) s.update(elapsed_ms);
}

void Spiders::setSpiderProperties(size_t index, vec2 position, vec2 velocity, vec2 scale)
{
	m_spiders[index].set_position(position);
	m_spiders[index].set_velocity(velocity);
	m_spiders[index].set_scale(scale);
}

void Spiders::destroy()
{
	for (auto& spider : m_spiders)
		spider.destroy();
}
