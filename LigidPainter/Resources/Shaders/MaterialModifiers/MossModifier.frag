/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#version 400 core

/* Color */
uniform vec3 mossColorBack = vec3(0.19,0.19,0.13);
uniform vec3 mossColorFront = vec3(0.21,0.27,0.01); // Front layer
uniform vec3 dirtColor = vec3(0.27,0.22,0.15);

/* Droplets */
uniform  float dropletsCount = 10.2;
uniform  float dropletsOpacityJitter = 1.;
uniform  float dropletsSize = 1.5;

/* Front Layer */
uniform float frontLayerStrength = 1.;
uniform float frontLayerScale = 0.5;

/* Lighting */
uniform float lightStrength = 1.;
uniform float darkeningStrength = 1.;

/* Noise */
uniform float noiseStrength = 1.;

/* Moss Properties*/
uniform float scale = 10.;

/* Element Properties */
uniform float wetness = 0.4;
uniform float metallic = 0.0;
uniform float height = 1.0; // 0 - 1

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;

/* Fragment Inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Outputs */
out vec4 fragColor;

/**
This tab contains all the necessary noise functions required to model a cloud shape.
*/

// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))

vec3 hash33(vec3 p)
{
	uvec3 q = uvec3(ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z)*UI3;
	return -1. + 2. * vec3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

// Gradient noise by iq (modified to be tileable)
float gradientNoise(vec3 x, float freq)
{
    // grid
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    // quintic interpolant
    vec3 u = w * w * w * (w * (w * 6. - 15.) + 10.);

    
    // gradients
    vec3 ga = hash33(mod(p + vec3(0., 0., 0.), freq));
    vec3 gb = hash33(mod(p + vec3(1., 0., 0.), freq));
    vec3 gc = hash33(mod(p + vec3(0., 1., 0.), freq));
    vec3 gd = hash33(mod(p + vec3(1., 1., 0.), freq));
    vec3 ge = hash33(mod(p + vec3(0., 0., 1.), freq));
    vec3 gf = hash33(mod(p + vec3(1., 0., 1.), freq));
    vec3 gg = hash33(mod(p + vec3(0., 1., 1.), freq));
    vec3 gh = hash33(mod(p + vec3(1., 1., 1.), freq));
    
    // projections
    float va = dot(ga, w - vec3(0., 0., 0.));
    float vb = dot(gb, w - vec3(1., 0., 0.));
    float vc = dot(gc, w - vec3(0., 1., 0.));
    float vd = dot(gd, w - vec3(1., 1., 0.));
    float ve = dot(ge, w - vec3(0., 0., 1.));
    float vf = dot(gf, w - vec3(1., 0., 1.));
    float vg = dot(gg, w - vec3(0., 1., 1.));
    float vh = dot(gh, w - vec3(1., 1., 1.));
	
    // interpolation
    return va + 
           u.x * (vb - va) + 
           u.y * (vc - va) + 
           u.z * (ve - va) + 
           u.x * u.y * (va - vb - vc + vd) + 
           u.y * u.z * (va - vc - ve + vg) + 
           u.z * u.x * (va - vb - ve + vf) + 
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

// Tileable 3D worley noise
float worleyNoise(vec3 uv, float freq)
{    
    vec3 id = floor(uv);
    vec3 p = fract(uv);
    
    float minDist = 10000.;
    for (float x = -1.; x <= 1.; ++x)
    {
        for(float y = -1.; y <= 1.; ++y)
        {
            for(float z = -1.; z <= 1.; ++z)
            {
                vec3 offset = vec3(x, y, z);
            	vec3 h = hash33(mod(id + offset, vec3(freq))) * .5 + .5;
    			h += offset;
            	vec3 d = p - h;
           		minDist = min(minDist, dot(d, d));
            }
        }
    }
    
    // inverted worley noise
    return 1. - minDist;
}

// Fbm for Perlin noise based on iq's blog
float perlinfbm(vec3 p, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    return noise;
}

// Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float worleyFbm(vec3 p, float freq)
{
    return worleyNoise(p*freq, freq) * .625 +
        	 worleyNoise(p*freq*2., freq*2.) * .25 +
        	 worleyNoise(p*freq*4., freq*4.) * .125;
}

float getWorleyNoise( vec2 uv )
{

    vec4 col = vec4(0.);
    
    float slices = 128.; // number of layers of the 3d texture
    float freq = 4.;
    
    float pfbm= mix(1., perlinfbm(vec3(uv, floor(slices)/slices), 4., 7), .5);
    pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise
    
    col.g += worleyFbm(vec3(uv, floor(slices)/slices), freq);
    col.b += worleyFbm(vec3(uv, floor(slices)/slices), freq*2.);
    col.a += worleyFbm(vec3(uv, floor(slices)/slices), freq*4.);
    col.r += remap(pfbm, 0., 1., col.g, 1.); // perlin-worley
    
    return 1. - col.z;
}

vec2 rotate(vec2 uv, float a)
{
	return vec2(uv.x*cos(a)-uv.y*sin(a),uv.y*cos(a)+uv.x*sin(a));
}

const int firstOctave = 3;
const int octaves = 8;
const float persistence = 0.6;

//Not able to use bit operator like <<, so use alternative noise function from YoYo
//
//https://www.shadertoy.com/view/Mls3RS
//
//And it is a better realization I think
float noise(int x,int y)
{   
    float fx = float(x);
    float fy = float(y);
    
    return 2.0 * fract(sin(dot(vec2(fx, fy) ,vec2(12.9898,78.233))) * 43758.5453) - 1.0;
}

float smoothNoise(int x,int y)
{
    return noise(x,y)/4.0+(noise(x+1,y)+noise(x-1,y)+noise(x,y+1)+noise(x,y-1))/8.0+(noise(x+1,y+1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x-1,y-1))/16.0;
}

float COSInterpolation(float x,float y,float n)
{
    float r = n*3.1415926;
    float f = (1.0-cos(r))*0.5;
    return x*(1.0-f)+y*f;
    
}

float InterpolationNoise(float x, float y)
{
    int ix = int(x);
    int iy = int(y);
    float fracx = x-float(int(x));
    float fracy = y-float(int(y));
    
    float v1 = smoothNoise(ix,iy);
    float v2 = smoothNoise(ix+1,iy);
    float v3 = smoothNoise(ix,iy+1);
    float v4 = smoothNoise(ix+1,iy+1);
    
   	float i1 = COSInterpolation(v1,v2,fracx);
    float i2 = COSInterpolation(v3,v4,fracx);
    
    return COSInterpolation(i1,i2,fracy);
    
}

float PerlinNoise2D(float x,float y)
{
    float sum = 0.0;
    float frequency =0.0;
    float amplitude = 0.0;
    for(int i=firstOctave;i<octaves + firstOctave;i++)
    {
        frequency = pow(2.0,float(i));
        amplitude = pow(persistence,float(i));
        sum = sum + InterpolationNoise(x*frequency,y*frequency)*amplitude;
    }
    
    return sum;
}

float getPerlin(vec2 uv)
{
   
    float x = uv.x;
    float y = uv.y;
	//fragColor = vec4(noise(x,y),noise(x,y),noise(x,y),1);
    float noise = 0.3+0.7*PerlinNoise2D(x,y);
    return noise;
}

const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(vec2 xy)
{
      return fract(tan(distance(xy*PHI, xy))*xy.x *PHI);
}

vec3 hash3( vec2 p )
{
    vec3 q = vec3( dot(p,vec2(127.1,311.7)), 
				   dot(p,vec2(269.5,183.3)), 
				   dot(p,vec2(419.2,371.9)) );
	return fract(sin(q)*43758.5453);
}

float voronoise( in vec2 p, float u, float v )
{
	float k = 1.0+63.0*pow(1.0-v,6.0);

    vec2 i = floor(p);
    vec2 f = fract(p);
    
	vec2 a = vec2(0.0,0.0);
    for( int y=-2; y<=2; y++ )
    for( int x=-2; x<=2; x++ )
    {
        vec2  g = vec2( x, y );
		vec3  o = hash3( i + g )*vec3(u,u,1.0);
		vec2  d = g - f + o.xy;
		float w = pow( 1.0-smoothstep(0.0,1.414,length(d)), k );
		a += vec2(o.z*w,w);
    }
	
    return a.x/a.y;
}

float getVoronoi( vec2 uv )
{

    vec2 p = vec2(0.6);
    
	
	p = p*p*(3.0-2.0*p);
	p = p*p*(3.0-2.0*p);
	p = p*p*(3.0-2.0*p);
	
	return voronoise( 24.0*uv, p.x, p.y );

}

float hash(in vec2 p)
{ 
      return fract(sin(dot(p.xy, vec2(12.9898,78.233))) * 43758.5453123); 
}

float hash2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


float dotNoise2D(in float x, in float y, in float fractionalMaxDotSize, in float dDensity)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;

    if (hash2D(vec2(integer_x+1.0, integer_y +1.0)) > dDensity)
       {return 0.0;}

    float xoffset = (hash2D(vec2(integer_x, integer_y)) -0.5);
    float yoffset = (hash2D(vec2(integer_x+1.0, integer_y)) - 0.5);
    float dotSize = 0.5 * fractionalMaxDotSize * max(0.25,hash2D(vec2(integer_x, integer_y+1.0)));

    vec2 truePos = vec2 (0.5 + xoffset * (1.0 - 2.0 * dotSize) , 0.5 + yoffset * (1.0 -2.0 * dotSize));

    float distance = length(truePos - vec2(fractional_x, fractional_y));

    return 1.0 - smoothstep(0.3 * dotSize, 1.0* dotSize, distance);

}

float DotNoise2D(in vec2 coord, in float wavelength, in float fractionalMaxDotSize, in float dDensity)
{
   return dotNoise2D(coord.x/wavelength, coord.y/wavelength, fractionalMaxDotSize, dDensity);
}






float getDroplets(vec2 uv){
    float splash_speed = 1.;
    float transform = 0.;
    int steps = 5;

    float rain = 0.;
    float base_rate = 6.0 + 3.0 * dropletsCount + 4.0 * (splash_speed - 1.0);
    float base_density = 0.6 * dropletsCount + 0.4 * (splash_speed -1.0);

    float opacityJitter = 1. - dropletsOpacityJitter;

    for(int j = 0; j < steps;j++){

        
        float hash = hash(vec2(float(j),float(j)*1.785));
        float opacity = 1. * hash;
        float opacityGap = opacity - 1.;
        opacity -= opacityGap * opacityJitter;        
        
        for(int i = 0; i < 4;i++){
        
            float time_fact = (sin(base_rate + (1.570*float(i))));
            time_fact = smoothstep(0.0,1.0,time_fact);
            rain+=(DotNoise2D(uv.xy, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity));
        }
            
    }
    
    return rain;
}


void main()
{
    vec3 aMossColorBack = mossColorBack;
    vec3 aMossColorFront = mossColorFront;
    vec3 aDirtColor = dirtColor;

    if(state != 0){
        aMossColorBack = vec3(0.2, 0.2 ,0.01);
        aMossColorFront = vec3(0.21, 0.27 ,0.01); // Front layer
        aDirtColor = vec3(0.27, 0.22, 0.15);
    }

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = TexCoords * scale;
    uv.x*=2.;

    float worley = getWorleyNoise(uv);
    
    vec3 baseColor = mix(aMossColorBack, aMossColorBack/1.2, worley);
    
    float voronoi = getVoronoi(uv / 5.);
    baseColor = mix(baseColor, aDirtColor, voronoi*2.);

    for(int i = 0; i < 4; i++){
        worley = getWorleyNoise(rotate(uv, 35. * float(i)));
        baseColor = mix(baseColor, aMossColorBack, worley);
    }
    
    worley = getWorleyNoise(rotate(uv * frontLayerScale, 95.));
    float worley2 = getWorleyNoise(rotate(uv / 2. + 0.1, 95.));

    vec3 absColor = mix(baseColor, aMossColorFront/2., worley * frontLayerStrength);
    absColor = mix(absColor, aMossColorFront, worley2/2.);

    float perlin = getPerlin(uv / 5.);
    float perlin2 = getPerlin(uv + 0.2);

    vec3 lightColor = mix(absColor, absColor*2.5, perlin * lightStrength);
    vec3 darkColor = mix(lightColor, lightColor/10.5, perlin2 * darkeningStrength);

    float noise = gold_noise(uv/5.);
        
    vec3 noisedColor = mix(darkColor, darkColor/1.4, noise * noiseStrength);

    voronoi = getVoronoi(uv/5. + 0.3);
    
    vec3 moss = mix(noisedColor, noisedColor*1.2, voronoi);
    
    float droplets = getDroplets(vec2((uv/(5.*2.)).x * 4., (uv/(5.*2.)).y)); 
    
    vec3 floweredMoss = mix(moss, vec3(0.,0.,0.), droplets/9.);
    
    fragColor = vec4(floweredMoss, 1.);
    
    /* Roughness */
    if(state == 1){
        fragColor.rgb = vec3(1. - fragColor.r * wetness);
    }
    
    
    /* Metallic */
    else if(state == 2){
        fragColor.rgb = vec3(fragColor.r * metallic);
    }
    
    /* Normal Map */
    else if(state == 3){

    }
    
    /* Height */
    else if(state == 4){
        fragColor.rgb = mix(vec3(fragColor.r * 2.) , vec3(1. - fragColor.r * 2.), height) + vec3(worley * frontLayerStrength) / 8.;
    }
    
    /* Ambient Occlusion */
    else if(state == 5){
        fragColor.rgb = mix(vec3(0.9 - fragColor.r ) , vec3(1. - fragColor.r), worley * frontLayerStrength);
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    vec3 clrResult = mix(fragColor.rgb, texture(previousTxtr, TexCoords).rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}