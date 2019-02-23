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
	m_position = initialPosition;
	m_velocity = {xVel, 0.0f};
}

void Character::set_dimensions(Texture* texture, float spriteSheetWidth, float spriteSheetHeight, int xTrim, int yTrim)
{
	width = (texture->width / spriteSheetWidth - xTrim) * m_scale.x;
	height = (texture->height / spriteSheetHeight - yTrim) * m_scale.y;
}

// Call if init() was successful
// Releases all graphics resources
void Character::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();

	free(characterState);
}

void Character::set_acceleration(vec2 acc) {
	m_acceleration = acc;
}

vec2 Character::get_acceleration() {
	return m_acceleration;
}

void Character::set_velocity(vec2 vel) {
	m_velocity = vel;
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

void Character::freeze() {
	preFreezeState = characterState->currentState;
	characterState->changeState(frozen);
	m_frozen = true;
}

void Character::unfreeze() {
	characterState->changeState(thawing);
	characterState->changeState(preFreezeState);
	m_frozen = false;
}

void Character::move() {
	if (characterState->currentState != frozen)
		m_position = add(m_position, m_velocity);
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

void Character::set_position(vec2 pos) {
	m_position = pos;
}

bool Character::is_alive()const {
	return characterState->currentState != dead;
}

void Character::kill() {
	characterState->changeState(dead);
}

bool Character::is_invincible()const {
	return isInvincible;
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
		{dead, reviving, 0},
		{reviving, idle, 0}
	};

	characterState = new StateTree(edges, idle);
}

