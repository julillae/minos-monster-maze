#include "../include/flashMessage.hpp"

// stlib
#include <stdio.h>
#include <string.h>

bool FlashMessage::init(int levelNumber)
{
    m_texture.id = 0;

    std::string fileName = "level";
    std::string image_number = std::to_string(levelNumber);
    fileName += image_number;
    fileName += "-prompt.png";
    fprintf(stderr, "%s\n", fileName.c_str());

    std::string texturePath(textures_path());
    const char* textureFile = (texturePath + fileName).c_str();

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	if (!RenderManager::set_render_data(&m_texture, this)) return false;

    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;

	m_rotation = 0.f;
    m_scale = {1.f, 1.f};
    set_flash_time();
	return true;
}

void FlashMessage::draw(const mat3& projection)
{
    if (numFlashes < maxFlashes && get_time_since_flash() > m_flash_duration) {
        set_flash_time();
        set_visibility(is_hidden);
        numFlashes++;
    }

    RenderManager::init_drawing_data(m_position, m_rotation, m_scale, this);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture.id);

    // Set opacity
    GLint is_hidden_uloc = glGetUniformLocation(effect.program, "is_hidden");
    glUniform1f(is_hidden_uloc, is_hidden);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // magnifies texture to avoid it being blurry when scaled
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void FlashMessage::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}

void FlashMessage::set_position(vec2 pos) {
    m_position = pos;
}

void FlashMessage::set_visibility(bool is_visible) {
    is_hidden = !is_visible;
}

void FlashMessage::set_flash_time() {
    m_flash_time = glfwGetTime();
}

float FlashMessage::get_time_since_flash() {
    return glfwGetTime() - m_flash_time;
}