// Header
#include "../include/characters/character.hpp"
#include "../include/physics.hpp"

bool Character::init(vec2 initialPosition, Physics * physicsHandler) {
	return false;
}

void Character::set_properties(vec2 initialPosition, float scaleFactor, float xVel)
{
	m_scale.x = scaleFactor;
	m_scale.y = scaleFactor;
	m_rotation = 0.f;
	m_is_alive = true;
	m_position = initialPosition;
	m_velocity = {xVel, 0.0f};
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
	return m_is_alive;
}

void Character::kill() {
	m_is_alive = false;
}