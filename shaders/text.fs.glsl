#version 330 core
in vec2 texCoords;
out vec4 color;
uniform bool is_hidden;

uniform sampler2D text;
uniform vec3 textColor;

vec4 hide(vec4 in_color)
{
    vec4 color = in_color;
    color.a = 0.0f;
    return color;

}

void main()
{
    vec4 sampler = vec4(0.0, 0.0, 0.0, texture(text, texCoords).r);
    color = vec4(textColor, 1.0) * sampler;

    if (is_hidden) color = hide(color);
}