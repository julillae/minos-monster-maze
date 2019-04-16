#include "../include/flashMessage.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <sstream>

bool FlashMessage::init(int levelNumber)
{
    m_texture.id = 0;

    std::string fileName = "level";
    std::string image_number = std::to_string(levelNumber);
    fileName += image_number;
    fileName += "-prompt.png";

    std::string texturePath(textures_path());
    std::string fullPath = texturePath + fileName;
    const char* textureFile = fullPath.c_str();

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	if (!RenderManager::set_render_data(&m_texture, this)) return false;

    if (!effect.load_from_file(shader_path("visibility.vs.glsl"), shader_path("visibility.fs.glsl")))
        return false;

	m_rotation = 0.f;
    m_scale = {1.f, 1.f};
    set_visibility(true);
    set_flash_time();
	return true;
}

void FlashMessage::draw(const mat3& projection)
{
    if (get_time_since_flash() > m_flash_duration) {
        set_visibility(false);
    }

    RenderManager::draw_hide_texture(projection, m_position, m_rotation, m_scale, &m_texture, is_hidden, this);
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