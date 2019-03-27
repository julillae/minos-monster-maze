#version 400

//////////////////////
////INPUT VARIABLES///
/////////////////////
layout(location = 0)in vec4 in_position;
layout(location = 1)in vec2 in_texcoord;

/////////////////////
//OUTPUT VARIABLES//
////////////////////
out vec2 Position;
out vec2 texCoord;
//out vec3 l_pos;

////////////////////
//UNIFORM VARIABLES//
////////////////////
uniform mat3 transform;
//uniform mat4 viewMatrix;
uniform mat3 projection;
//uniform vec3 worldPosition;
uniform vec2 light_pos;


///////////////////////////////////////////////////////////
//VERTEX SHADER
///////////////////////////////////////////////////////////
void main(void)
{
    //calculate the position of the vertex against MVP matrices
    //mat3 mvp = projection*transform;
    //gl_Position = mvp*vec4(in_position, 1.0);
    //vec3 pos = vec3(in_position.xy, 1.0);
    vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);


    //gl_Position = in_position;

    //vec3 light_p = vec3(light_pos, 1.0);
    //l_pos = light_p;

    //Position = worldPosition + in_position;
    vec3 Po = projection * vec3(in_position.xy, 1.0);
    Position = Po.xy;
    texCoord = in_texcoord;
}