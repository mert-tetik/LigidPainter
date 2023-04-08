#version 330 core

uniform sampler2D inputTexture;

in vec2 texCoords;

out vec4 color;

uniform float normalStrength;
#define textureOffset 1.0

const float PI = 3.14159265;
    
// from https://iquilezles.org/articles/distfunctions
float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
    //TODO : Check if round corners effect texture rendering+
    // setup
    float t = 0.2 + 0.2 * sin(mod(0.75, 2.0 * PI) - 0.5 * PI);
    float iRadius = (0.05 + t)*1080;
    vec2 halfRes = vec2(0.5*1080);

    // compute box
    float b = udRoundBox( uv.xy*1080 - halfRes, halfRes, iRadius );
    
    // colorize (red / black )
	vec3 c = mix( vec3(1.0,0.0,0.0), vec3(0.0,0.0,0.0), smoothstep(0.0,1.0,b) );
        
    return c.r;
}

vec2 stdNormalMap(in vec2 uv) 
{
    float height = texture(inputTexture, uv).r;
    return -vec2(dFdx(height), dFdy(height));
}
void main()
{
    vec2 normal = stdNormalMap(texCoords);
    
    normal *= normalStrength;
    normal += 0.5;
    
    color = vec4(normal, 1., 1.);   
    if(roundUp(texCoords) < 0.05)
        color.a = 0.;
}