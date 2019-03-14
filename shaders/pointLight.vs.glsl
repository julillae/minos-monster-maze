#version 400

//////////////////////
////INPUT VARIABLES///
/////////////////////
layout(location = 0)in vec3 in_position;
layout(location = 1)in vec2 in_texcoord;

/////////////////////
//OUTPUT VARIABLES//
////////////////////
out vec3 Position;
out vec2 texCoord;

////////////////////
//UNIFORM VARIABLES//
////////////////////
uniform mat3 transform;
//uniform mat4 viewMatrix;
uniform mat3 projection;
//uniform vec3 worldPosition;

///////////////////////////////////////////////////////////
//VERTEX SHADER
///////////////////////////////////////////////////////////
void main(void)
{
    //calculate the position of the vertex against MVP matrices
    mat3 mvp = projection*transform;
    //gl_Position = mvp*vec4(in_position, 1.0);
    vec3 pos = mvp*vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);

    //Position = worldPosition + in_position;
    Position = in_position;
    texCoord = in_texcoord;
}