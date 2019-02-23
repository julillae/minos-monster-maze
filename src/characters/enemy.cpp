#include "../include/characters/enemy.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <typeinfo>
#include <cmath>

void Enemy::reset_position()
{
	m_position = m_initialPosition;
}

vec2 Enemy::get_bounding_box()const
{
    return { width, height };
}
