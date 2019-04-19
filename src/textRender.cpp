#include <utility>

//
// Created by Henry Deng on 2019-04-18.
//

#include "../include/textRender.hpp"


TextRender::TextRender(const char *path, int size) {
    FT_Library ftLibrary;
    if (FT_Init_FreeType(&ftLibrary)) {
        printf("ERROR::FREETYPE: init failed\n");
        exit(1);
    }

    FT_Face face;
    if (FT_New_Face(ftLibrary, path, 0, &face)) {
        printf("ERROR::FREETYPE: could not load font\n");
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // Generate texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
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
        // Now store character for later use
        Word word = {
                textureID,
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
    m_scale = { 1.f, -1.f };
    m_position = { 0.f, 0.f };

    effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl"));
}

void TextRender::setColour(vec3 colour) {
    m_colour = colour;
}

void TextRender::setPosition(vec2 position) {
    m_position = position;
}

void TextRender::setScale(vec2 scale) {
    m_scale = scale;
}

float TextRender::get_width(std::string text) {
    std::string::const_iterator c;
    float return_value = 0.f;
    Word ch;
    for (c = text.begin(); c != text.end(); c++)
    {
        ch = words[*c];
        return_value += (ch.advance >> 6);
    }
    return return_value+ch.size.x;
}

void TextRender::render(const mat3& projection, std::string text) {

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

    // Activate corresponding render state
    glUniform3f(glGetUniformLocation(effect.program, "textColor"), m_colour.x, m_colour.y, m_colour.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float x = 0;
    float y = 0;

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Word ch = words[*c];

        GLfloat xpos = x + ch.bearing.x;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y);

        GLfloat w = ch.size.x;
        GLfloat h = ch.size.y;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRender::draw(const mat3& projection) {
    return;
}

void TextRender::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}

void TextRender::set_visibility(bool is_visible)
{
    is_hidden = !is_visible;
}

