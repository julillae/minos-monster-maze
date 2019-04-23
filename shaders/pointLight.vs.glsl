#version 400

layout(location = 0) in vec4 in_position;

uniform vec2 light_pos;
uniform mat3 projection;
uniform vec2 origin_pos;


out vec2 uv;
out vec3 light_position;
out vec3 origin_position;
out int count;

void main()
{
    gl_Position = in_position;
    count = 1;
    light_position = vec3(light_pos, 1.0);
    origin_position = vec3(origin_pos, 1.0);
    uv = (in_position.xy + vec2(1.05, 1.05)) / 2.1;
}