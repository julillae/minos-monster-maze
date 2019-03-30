#version 400

uniform sampler2D screen_texture;
//uniform float time;
//uniform float dead_timer;
//uniform vec2 u_resolution; 
uniform vec2 light_pos;
in vec2 uv;
in vec4 gl_FragCoord;
//layout(location = 0) out vec4 color;
out vec4 outputColor;

//struct ShaderBufferStruct
//{
    //vec3 l_pos;
//};




vec3 calculatePointLight(vec2 lightPosition, vec3 Color)
{
    vec3 finalColor = vec3(0.0);

    vec2 l = lightPosition - gl_FragCoord.xy;
    float dist = length(l);

    float linear = 0;
    float quadratic = 0.00025/30.0;
    float constant = 0;
    float atten = min(1.0/(constant + (dist*linear)+ ((dist*dist)*quadratic)), 1.0);

    vec3 ambient = Color;

    vec3 lightIntensity = vec3(1,1,1);
    vec3 diffuse = (lightIntensity*ambient)*atten;

    finalColor += diffuse;

    return finalColor;
} 

////////////////////////////////////////////////////////////////////////////////
//Fragment ShaderBufferStruct
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
    //vec4 textureColor = texture(sampler0, texCoord);
    //outputColor = vec4(calculatePointLight(shaderBufferStruct.l_pos, vec3(1,1,1)), 1.0) * textureColor;
    //vec3 l_pos = vec3(light_pos, 1.0);

    //vec2 coord = distort(uv);

    vec4 texturecolor = texture(screen_texture, uv);
   
    outputColor = vec4(calculatePointLight(light_pos, vec3(1,1,1)), 1.0)*texturecolor;
}
// //////////////////////
// ////INPUT VARIABLES///
// /////////////////////
// in vec2 Position;
// in vec2 texCoord;
// //in vec3 l_pos;
// ///////////////////////
// ///OUTPUT VARIABLES///
// //////////////////////
// out vec4 outputColor;

// //struct ShaderBufferStruct
// //{
//     //vec3 l_pos;
// //};


// /////////////////////////
// ///UNIFORM VARIABLES/////
// /////////////////////////
// //uniform ShaderBufferStruct shaderBufferStruct;
// uniform vec2 light_pos;
// uniform sampler2D screen_texture;

// vec3 calculatePointLight(vec2 lightPosition, vec3 Color)
// {
//     vec3 finalColor = vec3(0.0);

//     vec2 l = lightPosition - Position;
//     float dist = length(l);

//     float linear = 0;
//     float quadratic = 0.00025;
//     float constant = 0;
//     float atten = min(1.0/(constant + (dist*linear)+ ((dist*dist)*quadratic)), 1.0);

//     vec3 ambient = Color;

//     vec3 lightIntensity = vec3(1,1,1);
//     vec3 diffuse = (lightIntensity*ambient)*atten;

//     finalColor += diffuse;

//     return finalColor;
// } 

// ////////////////////////////////////////////////////////////////////////////////
// //Fragment ShaderBufferStruct
// ////////////////////////////////////////////////////////////////////////////////
// void main(void)
// {
//     //vec4 textureColor = texture(sampler0, texCoord);
//     //outputColor = vec4(calculatePointLight(shaderBufferStruct.l_pos, vec3(1,1,1)), 1.0) * textureColor;
//     //vec3 l_pos = vec3(light_pos, 1.0);

//     //vec2 coord = distort(uv);

//     vec4 texturecolor = texture(screen_texture, texCoord);
//     //outputColor = in_color;


//     outputColor = vec4(calculatePointLight(light_pos, vec3(1,1,1)), 1.0)*texturecolor;

// }