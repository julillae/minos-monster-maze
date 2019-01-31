// Header
#include "../include/characters/player.hpp"

// internal

// stlib
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

bool Player::init(vec2 initialPosition)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("salmon.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, -z }; 
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		indices.push_back((uint16_t)idx[0]);
		indices.push_back((uint16_t)idx[1]);
		indices.push_back((uint16_t)idx[2]);
	}

	// Done reading
	fclose(mesh_file);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
		return false;
	
	// Setting initial values
	m_scale.x = -35.f;
	m_scale.y = 35.f;
	m_is_alive = true;
	m_num_indices = indices.size();
	h_direction = Direction::none;
	v_direction = Direction::none;
	//m_position = { 150.f, 200.f };
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
		onPlatform = (m_position.y >= fakeFloorPos - tolerance);
		double jVel = 0.f;
		if (v_direction == Direction::up && onPlatform) jVel = jumpVel;
		//switch (v_direction) {
			//case Direction::up: 
				//if (onPlatform) jVel = jumpVel;
				//vAcc = -accStep; break;
			//case Direction::down: vAcc = accStep; break;
			//default: vAcc = 0.f; break;
		//}
		switch (h_direction) {
			case Direction::left: hAcc = -accStep; break;
			case Direction::right: hAcc = accStep; break;
			default: hAcc = 0.f; break;
		}
		//if (vAcc > maxAcceleration) vAcc = maxAcceleration;
		//if (vAcc < -maxAcceleration) vAcc = -maxAcceleration;
		vAcc = gravityAcc;
		if (hAcc > maxAcceleration) hAcc = maxAcceleration;
		if (hAcc < -maxAcceleration) hAcc = -maxAcceleration;

		set_acceleration({ hAcc, vAcc });

		update_velocity(jVel);
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
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Salmon Color
	float color[] = { 1.f, 0.f, 0.0f };
	if (!m_is_alive) {
		color[1] = 0.f;
		color[2] = 0.f;
	}
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES,(GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}

// Simple bounding box collision check, 


void Player::set_acceleration(vec2 acc)
{
	currentAcceleration.x = acc.x; currentAcceleration.y = acc.y;
}

void Player::update_velocity(double jumpVel)
{
	currentVelocity.x += currentAcceleration.x;
	currentVelocity.y += currentAcceleration.y + jumpVel;

	//if (currentVelocity.y > maxVelocity) currentVelocity.y = maxVelocity;
	//if (currentVelocity.y < -maxVelocity) currentVelocity.y = -maxVelocity;
	if (currentVelocity.x > maxVelocity) currentVelocity.x = maxVelocity;
	if (currentVelocity.x < -maxVelocity) currentVelocity.x = -maxVelocity;

	if (currentAcceleration.x < tolerance && currentAcceleration.x > -tolerance)
		currentVelocity.x *= drag;
	//if (currentAcceleration.y < tolerance && currentAcceleration.y > -tolerance)
	//	currentVelocity.y *= drag;
}

void Player::move()
{
	m_position.x += currentVelocity.x; m_position.y += currentVelocity.y;
	if (m_position.y >= fakeFloorPos - tolerance) {
		m_position.y = fakeFloorPos;
		onPlatform = TRUE;
	}
	else {
		onPlatform = FALSE;
	}

}

void Player::set_direction(int key, int action)
{
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_DOWN: v_direction = Direction::down; break;
			case GLFW_KEY_UP: v_direction = Direction::up; break;
			case GLFW_KEY_LEFT: h_direction = Direction::left; break;
			case GLFW_KEY_RIGHT: h_direction = Direction::right; break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_DOWN:
				if (v_direction == Direction::down)
					v_direction = Direction::none; break;
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

