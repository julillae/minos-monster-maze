#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 texCoords;

uniform mat3 projection;
uniform mat3 transform;

void main()
{
    vec3 pos = projection * transform * vec3(vertex.xy, 1.0);
    gl_Position = vec4(pos, 1.0);
    texCoords = vertex.zw;
}