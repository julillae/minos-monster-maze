#include "../../include/menus/button.hpp"

void Button::set_selected(bool isSelected)
{
    this->isSelected = isSelected;
}

void Button::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}

void Button::set_position(vec2 position)
{
    m_position = position;
}











