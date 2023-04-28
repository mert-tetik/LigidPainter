#version 330 core

out vec4 color;

in vec2 Pos;
in vec2 TexCoords;

const float PI = 3.14159265;

uniform sampler2D txtr;

uniform int blurVal;
    
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

const float width = 1024;

vec4 boxBlur(int range){
    vec3 res = vec3(0);
    for(int x = 0; x < range/10; x++){
        for(int y = 0; y < range/10; y++){
            res += texture(txtr,vec2(Pos.x+((x*range/10-range)/width),Pos.y+((y*range/10-range)/width))).rgb / pow(range/10,2);
        }
    }
    return vec4(res,1);
}

void main(){
    color = boxBlur(blurVal);

    if(roundUp(TexCoords) < 0.05){
        color.a = 0.;
    }

    color.rgb *= 1. - (blurVal/1000.);
}   