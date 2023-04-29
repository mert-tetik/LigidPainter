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

const int LOD = 1,         // gaussian done on MIPmap at scale LOD
          sLOD = 1 << LOD; // tile size = 2^LOD


float gaussian(vec2 i,int range) {
    float sigma = float(range) * .25;
    return exp( -.5* dot(i/=sigma,i) ) / ( 6.28 * sigma*sigma );
}

vec4 blur(int range) {
    vec4 O = vec4(0); 
    int s = range/sLOD;
    
    for ( int i = 0; i < s*s; i++ ) {
        vec2 d = vec2(i%s, i/s)*float(sLOD) - float(range)/2.;
        O += gaussian(d,range) * textureLod( txtr, Pos + vec2(1./width) * d , float(LOD) );
    }
    
    return O / O.a;
}

void main(){
    color = blur(blurVal/2);

    if(roundUp(TexCoords) < 0.05){
        color.a = 0.;
    }

    color.rgb *= 1. - (blurVal/1000.);
}   