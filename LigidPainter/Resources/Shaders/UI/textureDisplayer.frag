#version 330 core

in vec2 TexCoords;

uniform sampler2D currentTexture;
uniform int roundCor;

layout(location = 0) out vec4 color;

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

float mu(vec2 a){return a.x*a.y;}
float CheckerSignMuFract(vec2 u)
{
    return sign(mu(.5-fract(u)));
}

void main() {
    float checker = CheckerSignMuFract(TexCoords*15.f);
    checker+=1.5;

    color = vec4(mix(vec3(checker),texture(currentTexture, TexCoords).rgb,texture(currentTexture, TexCoords).a) ,1.0); 

    if(roundCor == 1)
        color.a = roundUp(TexCoords);

}