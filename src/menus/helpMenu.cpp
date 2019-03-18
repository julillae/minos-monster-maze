#include "../../include/menus/helpMenu.hpp"

bool HelpMenu::init(vec2 position)
{
    const char* textureFile = textures_path("help-menu.png");

    if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

    if (!RenderManager::set_render_data(&m_texture, this)) return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("menu.vs.glsl"), shader_path("menu.fs.glsl")))
        return false;

    set_position(position);

    // Setting initial values, scale is negative to make it face the opposite way
    m_scale.x = 0.51f;
    m_scale.y = 0.51f;
    m_rotation = 0.f;

    return true;

}

void HelpMenu::draw(const mat3 &projection)
{
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
    GLint is_hide_uloc = glGetUniformLocation(effect.program, "is_hide");
    glUniform1f(is_hide_uloc, is_hide);

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

void HelpMenu::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}

void HelpMenu::set_position(vec2 position)
{
    m_position = position;
}

void HelpMenu::set_visibility(bool show)
{

    if (show)
    {
        is_hide = 0.f;
    } else {
        is_hide = 1.f;
    }

}





