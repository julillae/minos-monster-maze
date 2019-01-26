#include "enemy.hpp"

// Put implementations specific to Enemies (not applicable to Player)
// but common across both simple and complex enemies here

void Enemy::set_position(vec2 position)
{
	m_position = position;
}
