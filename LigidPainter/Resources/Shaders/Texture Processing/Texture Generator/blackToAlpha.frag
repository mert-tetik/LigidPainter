#version 330 core

uniform sampler2D inputTexture;

in vec2 texCoords;

out vec4 color;

uniform int displayingMode;

const float PI = 3.14159265;
    
float mu(vec2 a){return a.x*a.y;}
float CheckerSignMuFract(vec2 u)
{
    return sign(mu(.5-fract(u)));
}

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

void main()
{
    color = texture(inputTexture,texCoords);
    color.a *= max(max(texture(inputTexture, texCoords).r,texture(inputTexture, texCoords).g),texture(inputTexture, texCoords).b);

    

    if(displayingMode == 1){
        float checker = CheckerSignMuFract(texCoords*15.f);
        checker+=1.5;
        color = mix(vec4(vec3(checker),1),color,color.a);
        color.a = 1;
        if(roundUp(texCoords) < 0.05)
            color.a = 0.;
    } 
}