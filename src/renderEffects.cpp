#include "../include/renderEffects.hpp"

#include <iostream>

bool RenderEffects::init() {
	m_dead_time = -1;
	m_win_time = -1;
	m_rotation_end_time = -1;

	// Since we are not going to apply transformation to this screen geometry
	// The coordinates are set to fill the standard openGL window [-1, -1 .. 1, 1]
	// Make the size slightly larger then the screen to crop the boundary
	static const GLfloat screen_vertex_buffer_data[] = {
		-1.05f, -1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		1.05f,  1.05f, 0.0f,
	};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer_data), screen_vertex_buffer_data, GL_STATIC_DRAW);

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("water.vs.glsl"), shader_path("water.fs.glsl")))
		return false;

	return true;
}

void RenderEffects::destroy() {
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void RenderEffects::set_player_dead() {
	m_dead_time = glfwGetTime();
}

void RenderEffects::set_level_complete_time() {
	m_win_time = glfwGetTime();
}

void RenderEffects::set_rotation_end_time() {
	m_rotation_end_time = glfwGetTime();
}

void RenderEffects::reset_player_dead_time() {
	m_dead_time = -1;
}

void RenderEffects::reset_player_win_time() {
	m_win_time = -1;
}

void RenderEffects::reset_rotation_end_time() {
	m_rotation_end_time = -1;
}

float RenderEffects::get_time_since_death() const {
	return glfwGetTime() - m_dead_time;
}

float RenderEffects::get_time_since_level_complete() const {
	return glfwGetTime() - m_win_time;
}

float RenderEffects::get_time_since_rotation_end() const {
	if (m_rotation_end_time > 0)
		return glfwGetTime() - m_rotation_end_time;
	else return m_rotation_end_time;
}

void RenderEffects::draw(const mat3& projection) {
	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Setting shaders
	glUseProgram(effect.program);

	// Set screen_texture sampling to texture unit 0
	// Set clock
	GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(effect.program, "dead_timer");
	GLuint win_timer_uloc = glGetUniformLocation(effect.program, "win_timer");
	glUniform1i(screen_text_uloc, 0);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(dead_timer_uloc, (m_dead_time > 0) ? (float)((glfwGetTime() - m_dead_time) * 80.0f) : -1);
	glUniform1f(win_timer_uloc, (m_win_time > 0) ? (float)((glfwGetTime() - m_win_time) * 80.0f) : -1);

	// Draw the screen texture on the quad geometry
	// Setting vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
}
