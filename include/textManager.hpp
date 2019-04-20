//
// Created by Henry Deng on 2019-04-18.
//
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

    TextManager(std::string path, int size);
    void setColour(vec3 colour);
    void setPosition(vec2 position);
    void render(const mat3& projection, std::string text);
    void draw(const mat3& projection);
    void destroy();

private:
    std::map<GLchar, Word> words;
    vec3 m_colour{};

    GLuint vao{};
    GLuint vbo{};

    vec2 m_position{};
    vec2 m_scale{};
};


