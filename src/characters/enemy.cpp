#include "../include/characters/enemy.hpp"

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

Texture Enemy::enemy_texture;

bool Enemy::init(vec2 initialPosition, float bound)
{
	const char* textureFile = textures_path("minotaur_right.png");
	// printf("The type is %s\n", typeid(textures_path("minotaur_right.png")).name());
	set_texture(textureFile);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	float scaleFactor = 0.4f;
	m_scale.x = scaleFactor;
	m_scale.y = scaleFactor;
	width = enemy_texture.width * scaleFactor;
	height = enemy_texture.height * scaleFactor;
	m_rotation = 0.f;
	direction = right;
	m_is_alive = true;
	m_position = initialPosition;
	currentVelocity = {maxVelocity, 0.0f};
	stopBound = bound;
	m_initialPosition = initialPosition;

	return true;
}


// Called on each frame by World::update()
void Enemy::update(float ms)
{

	if (m_is_alive)
	{
		// Update enemy position based on fixed path here
		float step = currentVelocity.x;
		if (direction == right) {
			if ((m_position.x + step) >= (m_initialPosition.x + stopBound)) {
				direction = left;
				m_scale.x = -1 * m_scale.x;
			} else {
				m_position.x += step;
			}
		} else if (direction == left) {
			if ((m_position.x - step) <= m_initialPosition.x) {
				direction = right;
				m_scale.x = -1 * m_scale.x;
			} else {
				m_position.x -= step;
			}
		}

	}
	else
	{
		// If dead reset back to original position (for now)
		// set_position();
	}

}

void Enemy::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, enemy_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

bool Enemy::set_texture(const char* texturePath)
{
	//const char* file = texturePath;
	// Load shared texture
	if (!enemy_texture.is_valid())
	{
		if (!enemy_texture.load_from_file(texturePath))
		{
			fprintf(stderr, "Failed to load enemy texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = enemy_texture.width;
	float hr = enemy_texture.height;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f,  };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	return true;
}

void Enemy::freeze()
{
	m_frozen = true;
}

void Enemy::unfreeze()
{
	m_frozen = false;
}

void Enemy::move()
{
	m_position.x += currentVelocity.x; m_position.y += currentVelocity.y;
}

void Enemy::set_direction(Direction d)
{
	direction = d;
}

void Enemy::reset_position()
{
	m_position = m_initialPosition;
}

vec2 Enemy::get_bounding_box()const
{
    return { width, height };
}
