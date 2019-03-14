#include "../include/characters/spider.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

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
    int horizontalTrim = 16;
    int verticalTrim = 19;

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

    // kept in for debugging
    //    fprintf(stderr, "spider height: %f\n", height); // 39.00f
    //    fprintf(stderr, "spider width: %f\n", width); // 48.00f

	return true;
}

void Spider::draw(const mat3& projection)
{
    if (use_sprite)
    {
        set_animation();
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &spider_texture, this);

    } else
    {
        RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &b_texture, this);

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