#include "../include/characters/enemy.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <typeinfo>
#include <cmath>

// Put implementations specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here

bool Enemy::init(vec2 initialPosition, Physics * physicsHandler) 
{
	return false;
}

void Enemy::update(float ms)
{
	if (m_is_alive)	move();
}

void Enemy::draw(const mat3& projection)
{
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &m_texture, this);
	
}

void Enemy::freeze()
{
	m_frozen = true;
}

void Enemy::unfreeze()
{
	m_frozen = false;
}

void Enemy::reset_position()
{
	m_position = m_initialPosition;
}

vec2 Enemy::get_bounding_box()const
{
    return { width, height };
}
