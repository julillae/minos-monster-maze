// Header
#include "../include/characters/player.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <cmath>

Texture Player::fish_texture;

bool Player::init(vec2 initialPosition)
{
	
	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	//FILE* mesh_file = fopen(mesh_path("salmon.mesh"), "r");
	//if (mesh_file == nullptr)
	//	return false;

	// Reading vertices and colors
	//size_t num_vertices;
	//fscanf(mesh_file, "%zu\n", &num_vertices);
	//for (size_t i = 0; i < num_vertices; ++i)
	//{
		//float x, y, z;
		//float _u[3]; // unused
		//int r, g, b;
		//fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
		//Vertex vertex;
		//vertex.position = { x, y, -z }; 
		//vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		//vertices.push_back(vertex);
	//}

	// Reading associated indices
	//size_t num_indices;
	//fscanf(mesh_file, "%zu\n", &num_indices);
	//for (size_t i = 0; i < num_indices; ++i)
	//{
		//int idx[3];
		//fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		//indices.push_back((uint16_t)idx[0]);
		//indices.push_back((uint16_t)idx[1]);
		//indices.push_back((uint16_t)idx[2]);
	//}

	// Done reading
	//fclose(mesh_file);

	if (!fish_texture.is_valid())
	{
		if (!fish_texture.load_from_file(textures_path("ice.png")))
		{
			fprintf(stderr, "Failed to load turtle texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = fish_texture.width * 0.5f;
	float hr = fish_texture.height * 0.5f;

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);

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
	
	// Setting initial values
	m_scale.x = -50.f;
	m_scale.y = 50.f;
	m_is_alive = true;
	v_direction = Direction::none;
	m_position = initialPosition;
	m_rotation = 0.f;
	currentVelocity = {0.0, 0.0};

	return true;
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	float vAcc;
	float hAcc;
	float vVel;
	float hVel;

	if (m_is_alive)
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// UPDATE PLAYER POSITION HERE BASED ON KEY PRESSED (World::on_key())
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		switch (h_direction) {
			case Direction::left: hAcc = -accStep; break;
			case Direction::right: hAcc = accStep; break;
			default: hAcc = 0.f; break;
		}
		vAcc = gravityAcc;

		set_acceleration({ hAcc, vAcc });
		update_velocity();
		move();

	}
	else
	{
		// If dead we make it face upwards and sink deep down
		set_rotation(3.1415f);
		//move({ 0.f, step });
	}

}

void Player::draw(const mat3& projection)
{
	transform_begin();

	transform_translate(m_position);
	transform_scale(m_scale);
	transform_rotate(m_rotation);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

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
	glVertexAttribPointer(in_texcoord_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fish_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Salmon Color
	float color[] = { 1.f, 1.f, 1.f };
	//if (!m_is_alive) {
	//	color[1] = 0.f;
	//	color[2] = 0.f;
	//}

	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_SHORT, nullptr);
}

// Returns the local bounding coordinates scaled by the current size of the fish 
vec2 Player::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * fish_texture.width, std::fabs(m_scale.y) * fish_texture.height };
}

// Simple bounding box collision check, 


void Player::set_acceleration(vec2 acc)
{
	currentAcceleration.x = acc.x; currentAcceleration.y = acc.y;
}

void Player::update_velocity()
{
	currentVelocity.x += currentAcceleration.x;
	currentVelocity.y += currentAcceleration.y;

	if (currentVelocity.x > maxVelocity) currentVelocity.x = maxVelocity;
	if (currentVelocity.x < -maxVelocity) currentVelocity.x = -maxVelocity;

	if (currentAcceleration.x < tolerance && currentAcceleration.x > -tolerance)
		currentVelocity.x *= drag;
}

void Player::move()
{
	m_position.x += currentVelocity.x; m_position.y += currentVelocity.y;
	currentFloorPos = std::min(fakeFloorPos, currentFloorPos);
	if (m_position.y >= currentFloorPos - tolerance) {
		m_position.y = currentFloorPos;
		currentVelocity.y = 0.f;
		isOnPlatform = true;
	}
	else {
		set_in_free_fall();
	}

}

void Player::set_on_platform(double yPos) {
	isOnPlatform = true;
	currentFloorPos = yPos;
}

void Player::set_in_free_fall() {
	isOnPlatform = false;
	currentFloorPos = fakeFloorPos;
}

void Player::set_direction(int key, int action)
{
	if (action == GLFW_PRESS) {
		switch (key) {
			//case GLFW_KEY_DOWN: v_direction = Direction::down; break;
			case GLFW_KEY_UP: if (isOnPlatform) currentVelocity.y += jumpVel; break;
			case GLFW_KEY_LEFT: h_direction = Direction::left; break;
			case GLFW_KEY_RIGHT: h_direction = Direction::right; break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			//case GLFW_KEY_DOWN:
			//	if (v_direction == Direction::down)
			//		v_direction = Direction::none; break;
			case GLFW_KEY_UP: 
				if (v_direction == Direction::up)
					v_direction = Direction::none; break;
			case GLFW_KEY_LEFT: 
				if (h_direction == Direction::left)
					h_direction = Direction::none; break;
			case GLFW_KEY_RIGHT: 
				if (h_direction == Direction::right)
					h_direction = Direction::none; break;
		}
	}
}


void Player::set_rotation(float radians)
{
	m_rotation = radians;
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Player::kill()
{
	m_is_alive = false;
}

