// Header
#include "../include/characters/character.hpp"
#include "../include/physics.hpp"

bool Character::init(vec2 initialPosition, Physics * physicsHandler) {
	return false;
}

// Call if init() was successful
// Releases all graphics resources
void Character::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	free(characterState);
}

void Character::set_acceleration(vec2 acc) {
	m_acceleration.x = acc.x; m_acceleration.y = acc.y;
}

vec2 Character::get_acceleration() {
	return m_acceleration;
}

void Character::set_velocity(vec2 vel) {
	m_velocity.x = vel.x; m_velocity.y = vel.y;
}

vec2 Character::get_velocity() {
	return m_velocity;
}

vec2 Character::get_position()const {
	return m_position;
}

vec2 Character::get_scale()const {
	return m_scale;
}

void Character::move()
{
	m_position.x += m_velocity.x; m_position.y += m_velocity.y;
}

void Character::set_direction(Direction d)
{
	direction = d;
}

Direction Character::get_h_direction() {
	return direction;
}

void Character::set_rotation(float radians) {
	m_rotation = radians;
}

bool Character::is_alive()const {
	return characterState->currentState != dead;
}

void Character::kill() {
	characterState->changeState(dead);
}

void Character::initStateTree()
{
	vector<Edge> edges = {
		{idle, running, 1},
		{idle, jumping, 1},
		{idle, rising, 1},
		{idle, falling, 1},
		{idle, frozen, 1},
		{idle, dead, 1},
		{running, idle, 1},
		{running, jumping, 1},
		{running, rising, 1},
		{running, falling, 1},
		{running, frozen, 1},
		{running, dead, 1},
		{jumping, rising, 0},
		{jumping, frozen, 0},
		{jumping, dead, 0},
		{rising, idle, 1},
		{rising, falling, 1},
		{rising, frozen, 1},
		{rising, dead, 1},
		{falling, idle, 1},
		{falling, frozen, 1},
		{falling, dead, 1},
		{frozen, idle, 0},
		{frozen, running, 0},
		{frozen, jumping, 0},
		{frozen, rising, 0},
		{frozen, falling, 0},
		{dead, idle, 0}
	};

	characterState = new StateTree(edges, idle);
}
