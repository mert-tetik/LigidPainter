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

/* Colors */
uniform vec3 color1 = vec3(0.65, 0.65, 0.61);
uniform vec3 color2 = vec3(0.43, 0.4, 0.39);
uniform vec3 color3 = vec3(0.29, 0.22, 0.17);
uniform vec3 color4 = vec3(0.49, 0.3, 0.2);

/*Properties*/
uniform float scale = 30.;

/* Perlin Properties */
uniform int firstOctave = 3;
uniform int octaves = 8;
uniform float persistence = 0.6;

/* FBM Properties*/
uniform int fbmOctaves = 8;
uniform float fbmRoughness = 0.5;

/* Rust Properties */
uniform float rustRadius = 2.;
uniform float batteringStrength = 1.0;

/* Noise Properties*/
uniform float noiseStrength = 1.;

/* Element Properties */
uniform float wetness = 1.;
uniform float metallic = 1.;
uniform float height = 0.;

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

/* Fragment Output*/
out vec4 fragColor;

float hash(vec2 p) {
    p = fract(p * vec2(5.3983, 5.4427));
    p += dot(p, p + 47.57);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    // Smoothstep interpolation
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    float n00 = hash(i);
    float n01 = hash(i + vec2(0.0, 1.0));
    float n10 = hash(i + vec2(1.0, 0.0));
    float n11 = hash(i + vec2(1.0, 1.0));
    
    // Bilinear interpolation
    return mix(
        mix(n00, n10, u.x),
        mix(n01, n11, u.x),
        u.y
    );
}

// Fractional Brownian Motion (FBM) noise function
// roughness: (0.0, 1.0], default: 0.5
// octaves: number of noise octaves to combine
float fbm(vec2 p) {
    float amplitude = 1.0;
    float frequency = 1.0;
    float total = 0.0;
    float maxTotal = 0.0;
    
    for (int i = 0; i < fbmOctaves; ++i) {
        total += amplitude * noise(p * frequency);
        maxTotal += amplitude;
        
        frequency *= 2.0;
        amplitude *= fbmRoughness;
    }
    
    return total / maxTotal;
}

vec2 fbm2D( in vec2 p )
{
    return vec2( fbm(p.xy), fbm(p.yx) );
}

// Rusty metal texture mapping
float mapRustyMetalTexture(vec2 p) {   
    // Combining multiple layers of FBM
    vec2 fbm_0 = 4.0*(p + fbm2D(4.0*p));
    vec2 fbm_1 = p + fbm2D(fbm_0);
    vec2 fbm_2 = fbm2D(fbm_1);
    float f = dot(fbm_2 , vec2(1.0,0.0) );
    return f;
}

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

const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(vec2 xy)
{
      return fract(tan(distance(xy*PHI, xy))*xy.x *PHI);
}

vec2 rotate(vec2 uv, float a)
{
	return vec2(uv.x*cos(a)-uv.y*sin(a),uv.y*cos(a)+uv.x*sin(a));
}

float restriction(float value, float rest){
    if(value < rest)
        value -= rest - value;
    if(value < 0.)
        value = 0.;
    return value;
}

void main() {
    
    if(state != 0){
        color1 = vec3(0.0); //Metal
        color2 = vec3(0.2); //Damaged Metal
        color3 = vec3(0.8); //Brown Color
        color4 = vec3(1.0); //Rust Color
    }

    vec2 p = TexCoords * scale;
    
    float rustyNoise = mapRustyMetalTexture(p) * 1.3;
    float rustyNoise2 = mapRustyMetalTexture(p*4.);
    float rustyNoise3 = mapRustyMetalTexture(rotate(p, 30.));
    vec3 result = vec3(0);
    float perlin = getPerlin(p)*2.;
    float voronoi = getVoronoi(p/5.);
    float voronoi2 = getVoronoi(p/10.);

    float noise1 = rustyNoise;
    if(noise1 < 0.7)
        noise1 -= 0.7 - noise1;
    
    result = mix(color1, color2, noise1 * perlin * batteringStrength);
    result = mix(result, color2, (voronoi * perlin) / 4.);

    float noise2 = rustyNoise;
    noise2 = restriction(noise2, 0.8);
        
    result = mix(result, color3, noise2 * perlin * voronoi * batteringStrength);
   
    float noise3 = rustyNoise;
    noise3 = restriction(noise3, rustRadius);
        
    float noise4 = rustyNoise2;
    noise4 = restriction(noise4, rustRadius);
        
    result = mix(result, color4, noise3 * voronoi2);
    result = mix(result, color4, noise4);

    float rustAc;
    if(state != 2)
        rustAc = 1.;
    else
        rustAc = 1.2;

    
    float noise5 = rustyNoise3 * rustAc;
    if(noise5 < 1.2)
        noise5 -= 1.2 - noise5;
    if(noise5 < 0.)
        noise5 = 0.;
        
    result = mix(result, color4, noise5);
    
    voronoi2 = restriction(voronoi2, 0.9);
    
    result = mix(result, color3, voronoi2 * perlin * noise3);
    
    result = mix(result, color3, voronoi2/4.);

    float goldNoise = gold_noise(p);
    
    result = mix(result, result / 1.2, goldNoise * noiseStrength);

    // Output the final color
    fragColor = vec4(vec3(result * 1.1), 1.0);

    /*Roughness*/
    if(state == 1){
        fragColor.rgb = vec3(mix(1., fragColor.r , wetness));
    }
    
    /*Metallic*/
    if(state == 2){
        fragColor.rgb = vec3(mix(0., 1.-fragColor.r, metallic));
    }
    
    /*Normal Map*/
    if(state == 3){
    
    }
    
    /*Height*/
    if(state == 4){
        fragColor.rgb = mix(vec3(1. - fragColor.r) , vec3(fragColor.r), height);
    }
    
    /*Ambient Occlusion*/
    if(state == 5){
        fragColor.rgb = vec3(1. - fragColor.r + 0.3);
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    vec3 clrResult = mix(fragColor.rgb, texture(previousTxtr, TexCoords).rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}