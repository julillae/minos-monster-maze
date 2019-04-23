
#include "../include/mazeComponents/fire.hpp"


#include <cmath>

bool Fire::init()
{
    m_win_time = -1;

    static const GLfloat screen_vertex_buffer[] = {
		-1.05f, -1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		1.05f,  1.05f, 0.0f,
	};

    gl_flush_errors();
    
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer), screen_vertex_buffer, GL_STATIC_DRAW);

    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("pointLight.vs.glsl"), shader_path("pointLight.fs.glsl")))
        return false;

	return true;
}

void Fire::destroy() {
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
    effect.release();
}

void Fire::draw(const mat3& projection)
{
	 // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(effect.program);

    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
    GLuint time_uloc = glGetUniformLocation(effect.program, "time");
    GLint light_pos = glGetUniformLocation(effect.program, "light_pos");
    GLint at_door = glGetUniformLocation(effect.program, "is_at_door");
    GLint origin_pos = glGetUniformLocation(effect.program, "origin_pos");
    GLint alive = glGetUniformLocation(effect.program, "alive");
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLuint win_timer_uloc = glGetUniformLocation(effect.program, "win_timer");
    GLuint light_uloc = glGetUniformLocation(effect.program, "light_mode");
    glEnableVertexAttribArray(in_position_loc);
    glUniform1i(screen_text_uloc, 0);


    float o_position[] = {px, py}; 
    glUniform2fv(origin_pos, 1, o_position);

    glUniform1f(alive, isAlive);

    float l_position[] = {tx/2.f + px, ty/2.f + py};
    glUniform2fv(light_pos, 1, l_position);
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
    glUniform1i(at_door, is_At_Door);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    glUniform1f(win_timer_uloc, (m_win_time > 0) ? (float)((glfwGetTime() - m_win_time) * 80.0f) : -1);
    glUniform1i(light_uloc, isLightMode);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	
}


void Fire::originUpdate(float ox, float oy, float p_x, float p_y){
    tx = ox;
    ty = oy;
    px = p_x;
    py = p_y;
}

void Fire::set_player_dead(){
    isAlive = 0.f;
}

void Fire::reset_fire(){
    isAlive = 1.f;
    is_At_Door = 0.f;
}

void Fire::set_success(){
    is_At_Door = 1;
}

void Fire::set_light_level(int isLightOn){
    isLightMode = isLightOn;
}

void Fire::set_level_complete_time() {
	m_win_time = glfwGetTime();
}

void Fire::reset_player_win_time() {
	m_win_time = -1;
}