#include <utility>

#include "../include/textManager.hpp"


void TextManager::init(std::string path, int size) {
    // initialize the free type library
    FT_Library ftLibrary;
    if (FT_Init_FreeType(&ftLibrary)) {
        exit(1);
    }

    FT_Face face;
    if (FT_New_Face(ftLibrary, path.c_str(), 0, &face)) {
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // store words
        Word word = {
                texture,
                { static_cast<float>(face->glyph->bitmap.width),  static_cast<float>(face->glyph->bitmap.rows) },
                { static_cast<float>(face->glyph->bitmap_left),  static_cast<float>(face->glyph->bitmap_top) },
                static_cast<GLuint>(face->glyph->advance.x)
        };
        words.insert(std::pair<GLchar, Word>(c, word));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ftLibrary);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_colour = { 1.f, 1.f, 1.f };
    m_scale = { 0.4f, -0.4f };
    m_position = { 0.f, 0.f };

    effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl"));
}

void TextManager::setColour(vec3 colour) {
    m_colour = colour;
}

void TextManager::setPosition(vec2 position) {
    m_position = position;
}

void TextManager::render(const mat3& projection, std::string text) {

    transform_begin();
    transform_translate(m_position);
    transform_scale(m_scale);
    transform_end();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setting shaders
    glUseProgram(effect.program);

    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

    GLint is_hidden_uloc = glGetUniformLocation(effect.program, "is_hidden");
    glUniform1f(is_hidden_uloc, is_hidden);

    glUniform3f(glGetUniformLocation(effect.program, "textColor"), m_colour.x, m_colour.y, m_colour.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float x = 0;
    float y = 0;

    // Adapted from: https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
    std::string::const_iterator i;
    for (i = text.begin(); i != text.end(); i++)
    {
        Word wrd = words[*i];

        GLfloat x_pos = x + wrd.bearing.x;
        GLfloat y_pos = y - (wrd.size.y - wrd.bearing.y);

        GLfloat w = wrd.size.x;
        GLfloat h = wrd.size.y;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { x_pos,     y_pos + h,   0.0, 0.0 },
                { x_pos,     y_pos,       0.0, 1.0 },
                { x_pos + w, y_pos,       1.0, 1.0 },

                { x_pos,     y_pos + h,   0.0, 0.0 },
                { x_pos + w, y_pos,       1.0, 1.0 },
                { x_pos + w, y_pos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, wrd.texture);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (wrd.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextManager::draw(const mat3& projection) {
    return;
}

void TextManager::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;
}

void TextManager::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}
