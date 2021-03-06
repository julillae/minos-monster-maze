#version 330
// From vertex shader
in vec2 texcoord;
uniform bool is_hidden;

// Application data

uniform vec3 fcolor;
uniform sampler2D sampler0;

// Output color
layout(location = 0) out  vec4 color;

vec4 hide(vec4 in_color)
{
    vec4 color = in_color;
    color.a = 0.0f;
    return color;

}

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));

	if (is_hidden) color = hide(color);

}
