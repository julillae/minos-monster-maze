#include "../../include/menus/menu.hpp"

void Menu::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}

void Menu::set_position(vec2 position)
{
    m_position = position;
}

void Menu::draw_button(const mat3& projection, Texture* texture, bool isSelected)
{
    if (isSelected) {
        float color[] = { 0.f, 0.f, 0.f };
        RenderManager::draw_texture_color(projection, m_position, m_rotation, m_scale, color, texture, this);
    } else {
        float color[] = { 1.f, 1.f, 1.f };
        RenderManager::draw_texture_color(projection, m_position, m_rotation, m_scale, color, texture, this);

    }
}

void Menu::set_selected(bool isSelected)
{
    this->isSelected = isSelected;
}