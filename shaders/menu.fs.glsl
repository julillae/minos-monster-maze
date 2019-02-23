#version 330
// From vertex shader
in vec2 texcoord;
uniform float is_hide;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

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

	if (is_hide == 1) color = hide(color);

}
