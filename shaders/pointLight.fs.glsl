#version 400

//////////////////////
////INPUT VARIABLES///
/////////////////////
in vec3 Position;
in vec2 texCoord;

///////////////////////
///OUTPUT VARIABLES///
//////////////////////
out vec4 outputColor;

struct ShaderBufferStruct
{
    vec3 pointLightPosition;
};

/////////////////////////
///UNIFORM VARIABLES/////
/////////////////////////
uniform ShaderBufferStruct shaderBufferStruct;
uniform sampler2D sampler0;

vec3 calculatePointLight(vec3 lightPosition, vec3 Color)
{
    vec3 finalColor = vec3(0.0);

    vec3 l = lightPosition - Position;
    float dist = length(l);

    float linear = 0;
    float quadratic = 0.00025;
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
    vec4 textureColor = texture(sampler0, texCoord);
    outputColor = vec4(calculatePointLight(shaderBufferStruct.pointLightPosition, vec3(1,1,1)), 1.0) * textureColor;

}