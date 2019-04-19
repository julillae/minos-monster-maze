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
    GLuint textureID;
    vec2 size;
    vec2 bearing;
    GLuint advance;
};

class TextRender : public Renderable {
public:

    TextRender(const char* path, int size);
    void setColour(vec3 colour);
    void setPosition(vec2 position);
    void setScale(vec2 scale);
    void render(const mat3& projection, std::string text);
    void draw(const mat3& projection);
    float get_width(std::string text);
    void destroy();
    void set_visibility(bool is_visible);


private:
    std::map<GLchar, Word> words;
    vec3 m_colour{};

    GLuint vao{};
    GLuint vbo{};

    vec2 m_position{};
    vec2 m_scale{};

    bool is_hidden = false;

};


