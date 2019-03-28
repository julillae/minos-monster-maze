#include <stack>
#include <chrono>

#include "../include/characters/minotaur.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"
#include "../include/gameAI.hpp"
#include "../include/level.hpp"
#include "../include/a1.hpp"

// Put implementation for Minotaur enemy here

using Clock = std::chrono::high_resolution_clock;
Texture Minotaur::minotaur_texture;

bool Minotaur::init(vec2 initialPosition, Physics * physicsHandler)
{
    Smart::init(initialPosition, physicsHandler);

    const char* textureFile = textures_path("minotaur-sprite-sheet-cropped.png");
	if (!RenderManager::load_texture(textureFile, &minotaur_texture, this)) {
        return false;
    }

	float spriteSheetWidth = 9.0f;
	float spriteSheetHeight = 10.0f;
    int horizontalTrim = 0;
    int verticalTrim = 0;

    m_stopBound = 10.f;
    cycle_start = Clock::now();

	spriteSheet.init(&minotaur_texture, { spriteSheetWidth, spriteSheetHeight }, this);

	spriteSheet.set_render_data(this, 0);

	initStateTreeMinotaur();
	set_properties(initialPosition, 1.0f, speed);
	m_frozen = false;
	set_dimensions(&minotaur_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);
	characterState->changeState(idle);

	return true;
}

void Minotaur::update(float ms)
{
	if (is_alive() && !m_frozen && !isPlayerClose())
	{
        // Want to move in direction of player if within a certain distance. Will stop at stop_bound if player still within range, be idle
        // Also want to be able to rotate world?
        characterState->changeState(running);
		if (atBound()) {
            if (direction == Direction::left) {
                direction = Direction::right;
            } else {
                direction = Direction::left;
            }
            m_scale.x *= -1;
		}
	} else if (is_alive() && !m_frozen && isPlayerClose())
    {
        vec2 playerLoc = world->m_player.get_position();
        if (atBound())
        {
            characterState->changeState(idle);
        } else if (abs(playerLoc.x - m_position.x) <= 40.f)
        {
            characterState->changeState(swinging);
        } else
        {
            setFollowDirection();
            characterState->changeState(following);
        }        
    } 
    updateVelocity();
    move();
    handleBossRotation();
}

void Minotaur::updateVelocity()
{
    switch (characterState->currentState){
        case idle:
            m_velocity.x = 0.f;
            break;
        case running:
            if (direction == Direction::right) {
                m_velocity.x = maxHorzSpeed;
            } else {
                m_velocity.x = maxHorzSpeed * -1;
            }
            break;
        case following:
            if (direction == Direction::right) {
                m_velocity.x = maxHorzSpeed + 1.f;
            } else {
                m_velocity.x = (maxHorzSpeed + 1.f)* -1;
            }
            break;
        case swinging:
            m_velocity.x = 0.f;
            break;
        default:
            break;
    }
}

bool Minotaur::atBound()
{
    float vel_x = m_velocity.x;
    float vel_y = m_velocity.y;
    float right_bound = m_initialPosition.x + m_stopBound;
    float left_bound = m_initialPosition.x;
    float next_pos_x = m_position.x + vel_x;

    if ((next_pos_x >= right_bound) || (next_pos_x <= left_bound)) {
        return true;
    }
    return false;
}

void Minotaur::setFollowDirection()
{
    vec2 playerLoc = world->m_player.get_position();
    if (((playerLoc.x < m_position.x) && (m_velocity.x > 0)) ||
         ((playerLoc.x > m_position.x) && (m_velocity.x < 0)) ) {
        // switch direction to follow player if necessary
        if (direction == Direction::left) {
            direction = Direction::right;
        } else {
            direction = Direction::left;
        }
        m_scale.x *= -1;
    } 
}

bool Minotaur::isPlayerClose()
{
   vec2 playerLoc = world->m_player.get_position();
   if (abs(playerLoc.x - m_position.x) <= detectPlayerRange && abs(playerLoc.y - m_position.y) <= detectPlayerRange) {
       return true;
   }
   return false;
}

void Minotaur::set_bound(float bound) 
{
   m_stopBound = bound;
}

void Minotaur::handleBossRotation()
{
    float timeElapsed = getTimeElapsed();
    if (rotating && (timeElapsed >= rotate_duration)) {
        printf("Stopping boss rotation\n");
        world->boss_rotation_set(false);
        rotating = false;
        resetCycleStart();
    } else
     if (timeElapsed >= rotate_cycle_time) {
        printf("Starting boss rotation\n");
        world->boss_rotation_set(true);
        rotating = true;
        resetCycleStart();
    }
}

float Minotaur::getTimeElapsed() 
{
    auto now = Clock::now();
	float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - cycle_start)).count() / 1000;
    return elapsed_sec;
}

void Minotaur::resetCycleStart() {
    cycle_start = Clock::now();
}

void Minotaur::draw(const mat3& projection)
{
    set_animation();
	RenderManager::draw_texture(projection, m_position, m_rotation, m_scale, &minotaur_texture, this);

}

void Minotaur::set_animation()
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
                numTiles = 6;
                tileIndex = 45;
                break;
            case running:
            case following:
                numTiles = 8;
                tileIndex = 9;
                break;
            case swinging:
                numTiles = 9;
                tileIndex = 27;
                break;
            default:
                numTiles = 5;
                tileIndex = 0;

        }
    } else {
        isRepeat = false;
        numTiles = 6;
        tileIndex = 81;
 
    }

    // Increment animation time
    m_animTime += animSpeed;

    // Apply animation
    tileIndex = tileIndex + (int)m_animTime % numTiles;

    // do not repeat death animation
    if (!isRepeat && tileIndex == 81) is_anim_once = true;

    spriteSheet.update_render_data(this, tileIndex);
}

void Minotaur::initStateTreeMinotaur()
{
	vector<Edge> edges = {
		{idle, running, 1},
		{idle, jumping, 1},
		{idle, rising, 1},
		{idle, falling, 1},
		{idle, frozen, 1},
		{idle, dead, 1},
        {idle, following, 1},
        {idle, swinging, 1},
		{running, idle, 1},
		{running, jumping, 1},
		{running, falling, 1},
		{running, frozen, 1},
		{running, dead, 1},
        {running, following, 1},
        {running, swinging, 1},
		{jumping, rising, 0},
		{jumping, frozen, 0},
		{jumping, dead, 0},
		{rising, idle, 1},
		{rising, falling, 1},
		{rising, frozen, 1},
		{rising, dead, 1},
		{falling, landing, 1},
		{falling, frozen, 1},
		{falling, dead, 1},
		{landing, idle, 1},
		{landing, running, 1},
		{landing, jumping, 1},
		{landing, frozen, 1},
		{landing, dead, 1},
		{frozen, thawing},
		{thawing, idle, 0},
		{thawing, running, 0},
		{thawing, rising, 0},
		{thawing, falling, 0},
        {thawing, following, 0},
        {thawing, swinging, 0},
        {following, idle, 1},
        {following, running, 1},
        {following, frozen, 1},
        {following, swinging, 1},
        {swinging, idle, 1},
        {swinging, running, 1},
        {swinging, frozen, 1},
        {swinging, following, 1},
		{dead, reviving, 0},
		{reviving, idle, 0}
	};

	characterState = new StateTree(edges, idle);
}