// Header
#include "../include/characters/character.hpp"
#include "../include/physics.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

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
	set_invincibility(false);
}

void Character::set_dimensions(Texture* texture, float spriteSheetWidth, float spriteSheetHeight, int xTrim, int yTrim)
{
	width = (texture->width / spriteSheetWidth - xTrim) * m_scale.x;
	height = (texture->height / spriteSheetHeight - yTrim) * m_scale.y;
	boundingCircleRadius = sqrtf(pow(0.5*width, 2.f) + pow(0.5*height, 2.f));
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

void Character::set_scale(vec2 scale) {
	m_scale = scale;
}

void Character::freeze() {
	if (characterState->currentState != frozen)
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

void Character::set_invincibility(bool setting)
{
	isInvincible = setting;
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

void Character::set_world_vertex_coord()
{
	vertex_coords.clear();

	float rightSide = m_position.x + width / 2;
	float leftSide = m_position.x - width / 2;
	float topSide = m_position.y + height / 2;
	float bottomSide = m_position.y - height / 2;
	vec2 vert1b = { rightSide, topSide };
	vec2 vert2b = { rightSide, bottomSide };
	vec2 vert3b = { leftSide , bottomSide };
	vec2 vert4b = { leftSide, topSide };

	vertex_coords.push_back(vert1b);
	vertex_coords.push_back(vert2b);
	vertex_coords.push_back(vert3b);
	vertex_coords.push_back(vert4b);
}

