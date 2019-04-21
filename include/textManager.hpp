#pragma once

#include "../include/common.hpp"
#include <string.h>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H


struct Word {
    GLuint texture;
    vec2 size;
    vec2 bearing;
    GLuint advance;
};

class TextManager : public Renderable {
public:
    void init(std::string path, int size);
    void setColour(vec3 colour);
    void setPosition(vec2 position);
    void render(const mat3& projection, std::string text);
    void draw(const mat3& projection);
    void destroy();
    void set_visibility(bool is_visible);

private:
    std::map<GLchar, Word> words;
    vec3 m_colour{};

    GLuint vao{};
    GLuint vbo{};

    bool is_hidden;
};


