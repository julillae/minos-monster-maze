// Header
#include "../include/characters/character.hpp"

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

vec2 Character::get_position()const
{
	return m_position;
}

vec2 Character::get_scale()const
{
	return m_scale;
}

