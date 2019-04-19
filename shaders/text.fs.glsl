#version 330 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampler = vec4(0.0, 0.0, 0.0, texture(text, texCoords).r);
    color = vec4(textColor, 1.0) * sampler;
}