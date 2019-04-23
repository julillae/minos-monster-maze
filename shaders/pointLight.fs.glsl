#version 400

uniform sampler2D screen_texture;
uniform vec2 light_pos;
uniform vec2 origin_pos;
uniform float time;
uniform int is_at_door;
uniform float win_timer;
uniform float dead_timer;
uniform bool light_mode;
in vec2 uv;
in vec4 gl_FragCoord;
in vec3 light_position;
out vec4 outputColor;




vec3 calculatePointLight(vec2 lightPosition, vec3 Color, float is_at_door)
{
    vec3 finalColor = vec3(0.0);

    vec2 l = lightPosition - gl_FragCoord.xy;
    float dist = length(l);

    float linear = 0;
    float quadratic = 0.00025/(30.0-0.6*sin(time*3.0));
    
    if(is_at_door!=0){
        quadratic = -0.0000000025*(30.0*5.0) - (1.0/200000.0)*win_timer;
    }
    float constant = 0.8f;
    float atten = min(1.0/(constant + (dist*linear)+ ((dist*dist)*quadratic)), 1.0);

    vec3 ambient = Color;

    vec3 lightIntensity = vec3(1,1,1);
    vec3 diffuse = (lightIntensity*ambient)*atten;

    finalColor += diffuse;

    return finalColor;
}

vec4 fade_color(vec4 in_color) {
	vec4 color = in_color;

	if (dead_timer > 0)
    		color -= 0.1 * dead_timer * vec4(0.1, 0.1, 0.1, 0);

	if (win_timer > 0)
		color -= 0.1 * win_timer * vec4(0.1, 0.1, 0.1, 0);

	return color;
}

////////////////////////////////////////////////////////////////////////////////
//Fragment ShaderBufferStruct
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
    vec4 texturecolor = texture(screen_texture, uv);
    if (light_mode){
        outputColor = vec4(calculatePointLight(light_position.xy, vec3(1,1,1), is_at_door), 1.0)*texturecolor;
    }else{
        outputColor = fade_color(texturecolor);

    }
}