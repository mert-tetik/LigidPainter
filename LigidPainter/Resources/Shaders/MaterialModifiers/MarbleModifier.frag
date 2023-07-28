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

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

// variant of Vorocracks: https://shadertoy.com/view/lsVyRy
// integrated with cracks here: https://www.shadertoy.com/view/Xd3fRN

/* Marble Properties */
uniform vec3 color1 = vec3(0.);
uniform vec3 color2 = vec3(1.);
uniform float scale = 0.5;
uniform int checkerState = 1;

/* Crack Properties*/
uniform float ofs = .5;          // jitter Voronoi centers in -ofs ... 1. + ofs
uniform float crackDepth = 3.;
uniform float crackZebraScale = 1.;  // fractal shape of the fault zebra
uniform float crackZebraAmp = .67;
uniform float crackProfile = 1.;      // fault vertical shape  1.  .2 
uniform float crackSlope = 50.;       //                      10.  1.4
uniform float crackWidth = .0;
uniform float crackScale = 10.;

/* Noise Properties */
uniform int firstOctave = 3;
uniform int octaves = 8;
uniform float persistence = 0.6;
uniform float noiseStrength = 0.5;

/* Corners */
uniform float cornerBlackoutRadius = 0.2;
uniform float cornerBlackoutStrength = 5.;
uniform float cornerBlackoutNoiseSize = 8.;

/* Element Properties */
uniform float reflectiveness = 1.;
uniform float metallic = 0.;
uniform float height = 0.7;

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform int proceduralID;
uniform float proceduralScale;
uniform int proceduralInverted;

/* Fragment Inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;

// std int hash, inspired from https://www.shadertoy.com/view/XlXcW4
vec3 hash3( uvec3 x ) 
{
    #define scramble  x = ( (x>>8U) ^ x.yzx ) * 1103515245U // GLIB-C const
    scramble; scramble; scramble; 
    return vec3(x) / float(0xffffffffU) + 1e-30; // <- eps to fix a windows/angle bug
}

// === Voronoi =====================================================
// --- Base Voronoi. inspired by https://www.shadertoy.com/view/MslGD8

#define hash22(p)  fract( 18.5453 * sin( p * mat2(127.1,311.7,269.5,183.3)) )
#define disp(p) ( -ofs + (1.+2.*ofs) * hash22(p) )

vec3 voronoi( vec2 uv )  // returns len + id
{
    vec2 iu = floor(uv), v;
	float m = 1e9,d;
    for( int k=0; k < 25; k++ ) {
        vec2  p = iu + vec2(k%5-2,k/5-2),
            o = disp(p),
      	      r = p - uv + o;
		d = dot(r,r);
        if( d < m ) m = d, v = r;
    }

    return vec3( sqrt(m), v+uv );
}

// --- Voronoi distance to borders. inspired by https://www.shadertoy.com/view/ldl3W8
vec3 voronoiB( vec2 uv )  // returns len + id
{
    vec2 iu = floor(uv), C, P;
	float m = 1e9,d;
    for( int k=0; k < 25; k++ ) {
        vec2  p = iu + vec2(k%5-2,k/5-2),

              o = disp(p),
      	      r = p - uv + o;
		d = dot(r,r);
        if( d < m ) m = d, C = p-iu, P = r;
    }

    m = 1e9;
    
    for( int k=0; k < 25; k++ ) {
        vec2 p = iu+C + vec2(k%5-2,k/5-2),
		     o = disp(p),
             r = p-uv + o;

        if( dot(P-r,P-r)>1e-5 )
        m = min( m, .5*dot( (P+r), normalize(r-P) ) );
    }

    return vec3( m, P+uv );
}

// === pseudo Perlin noise =============================================
#define rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
int MOD = 1;  // type of Perlin noise
    
// --- 2D
#define hash21(p) fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453123)

float noise2x(vec2 p) {

    vec2 i = floor(p);
    vec2 f = fract(p); f = f * f * (3. - 2. * f); // smoothstep

    float v= mix( mix(hash21(i+vec2(0,0)),hash21(i+vec2(1,0)),f.x),
                  mix(hash21(i+vec2(0,1)),hash21(i+vec2(1,1)),f.x), f.y);
	return   MOD==0 ? v
	       : MOD==1 ? 2.*v-1.
           : MOD==2 ? abs(2.*v-1.)
                    : 1.-abs(2.*v-1.);
}

float fbm2(vec2 p) {
    float v = 0.,  a = .5;
    mat2 R = rot(.37);

    for (int i = 0; i < 9; i++, p*=2.,a/=2.) 
        p *= R,
        v += a * noise2x(p);

    return v;
}
#define noise22(p) vec2(noise2x(p),noise2x(p+17.7))
vec2 fbm22(vec2 p) {
    vec2 v = vec2(0);
    float a = .5;
    mat2 R = rot(.37);

    for (int i = 0; i < 6; i++, p*=2.,a/=2.) 
        p *= R,
        v += a * noise22(p);

    return v;
}
vec2 mfbm22(vec2 p) {  // multifractal fbm 
    vec2 v = vec2(1);
    float a = .5;
    mat2 R = rot(.37);

    for (int i = 0; i < 6; i++, p*=2.,a/=2.) 
        p *= R,
        //v *= 1.+noise22(p);
          v += v * a * noise22(p);

    return v-1.;
}

/*
// --- 3D 
#define hash31(p) fract(sin(dot(p,vec3(127.1,311.7, 74.7)))*43758.5453123)
float noise3(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p); f = f*f*(3.-2.*f); // smoothstep

    float v= mix( mix( mix(hash31(i+vec3(0,0,0)),hash31(i+vec3(1,0,0)),f.x),
                       mix(hash31(i+vec3(0,1,0)),hash31(i+vec3(1,1,0)),f.x), f.y), 
                  mix( mix(hash31(i+vec3(0,0,1)),hash31(i+vec3(1,0,1)),f.x),
                       mix(hash31(i+vec3(0,1,1)),hash31(i+vec3(1,1,1)),f.x), f.y), f.z);
	return   MOD==0 ? v
	       : MOD==1 ? 2.*v-1.
           : MOD==2 ? abs(2.*v-1.)
                    : 1.-abs(2.*v-1.);
}

float fbm3(vec3 p) {
    float v = 0.,  a = .5;
    mat2 R = rot(.37);

    for (int i = 0; i < 9; i++, p*=2.,a/=2.) 
        p.xy *= R, p.yz *= R,
        v += a * noise3(p);

    return v;
}
*/
    
// ======================================================

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
    return 0.3+0.7*PerlinNoise2D(x,y);
}

#define cellsize 50.0

float getEdges(vec2 uv) {

    uv-= 0.06;
    uv*=8.;
    
    vec2 orig = floor(uv)+0.5;
    
    vec2 c01 = vec2(-0.5, 0.5);
    vec2 c11 = vec2( 0.5, 0.5);
    vec2 c10 = vec2( 0.5,-0.5);
    vec2 c00 = vec2(-0.5,-0.5);
    
    vec4 fragColor = max(vec4(1.0 - cornerBlackoutRadius * length(orig-uv+c00)), vec4(0.0));
    fragColor = max(vec4(1.0 - cornerBlackoutRadius * length(orig-uv+c01)), fragColor);
    fragColor = max(vec4(1.0 - cornerBlackoutRadius * length(orig-uv+c11)), fragColor);
    fragColor = max(vec4(1.0 - cornerBlackoutRadius * length(orig-uv+c10)), fragColor);

    return 1. - fragColor.r;
}

float getChecker(vec2 uv)
{
    uv/=40.;
    
    if(checkerState == 0)
        return 0.;
    
    else if(checkerState == 1){
        bool x = mod(uv.x, 0.1) < 0.05;
        bool y = mod(uv.y, 0.1) < 0.05;
        bool c = x != y;
        
        return float(c);
    }
    
    else if(checkerState == 2){
        float x = mod(uv.x, 0.1);
        float y = mod(uv.y, 0.1);
    
        float xx = smoothstep(0.049, 0.051, x) * (1.0 - smoothstep(0.099, 0.11, x));
        float yy = smoothstep(0.049, 0.051, y) * (1.0 - smoothstep(0.099, 0.11, y));

        float c = mix(xx, yy, 0.5);
        return float(c);
    }
    
    else
        return 1.;
    
	
}

float hash(in vec2 p)
{ 
      return fract(sin(dot(p.xy, vec2(12.9898,78.233))) * 43758.5453123); 
}

float noise (in vec2 p)
{
  vec2 i = floor(p);
  vec2 f = fract(p);

  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));

  vec2 u = f * f * (3.0 - 2.0 * f);

  return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(in vec2 p)
{
  float value = 0.0;
  float amplitude = 1.0;
  float frequency = 2.;

  for (int i = 0; i < 16; i++)
  {
    value += amplitude * noise(p);
    p *= 3.;
    amplitude *= .5;
  }
    
  return value;
}

vec4 mainNoise(vec2 uv)
{
    return vec4( fbm(uv + (fbm(uv) * 5.0) ) );
}

float getNoise2(vec2 uv){
    return 0.25 * mainNoise(1.0 * uv).r; 
}

void main()
{
    vec2 texCoords;
    vec2 uv = TexCoords * crackScale;
    texCoords *= scale;
    
    vec2 I = floor(uv/2.);         
    vec3 H0;
    fragColor-=fragColor;
    
    const float RATIO = 1.;              // stone length/width ratio

    // Calculate the cracks
    for(float i=0.; i < crackDepth ; i++) {
        
        vec2 V =  uv / vec2(RATIO,1); // voronoi cell shape
        vec2 D = crackZebraAmp * fbm22(uv / crackZebraScale) * crackZebraScale;
        
        vec3  H = voronoiB( V + D ); 
       
        if (i==0.) 
            H0=H;
        
        float d = H.x;                                // distance to cracks
        d = min( 1., crackSlope * pow(max(0., d - crackWidth), crackProfile));
  
        fragColor += vec4(1.-d) / exp2(i);
        uv *= 1.5 * rot(.37);
    }

    
   // The checker effect
    float checker = getChecker(texCoords * 16.);

    if(state == 0){
       if(checker > 0.7)
           fragColor = 1. - fragColor; 
       if(checker > 0.3 && checker < 0.7)
           fragColor += checker;
   }
   
   if(state == 4){
       fragColor += checker/6.;
   }
    
      
    // Add a little noise
    fragColor.rgb = mix(fragColor.rgb,vec3(1.) - fragColor.rgb,getPerlin(texCoords * 4.) * noiseStrength);
    
    // Black out the corners
    if((checkerState == 1 || checkerState == 2) && (state == 0)){
        float edges = getEdges(texCoords) * cornerBlackoutStrength;
        if(edges < 0.6){
            edges -= 0.6 -edges;
        }
            
        fragColor.rgb *= mix(fragColor.rgb, vec3(getNoise2(texCoords * cornerBlackoutNoiseSize )), edges);
        //fragColor.rgb = vec3(edges);
    }
    
    /* Albedo */
    if(state == 0)
        fragColor.rgb = mix(color1, color2, fragColor.r);
    
    /* Roughness */
    if(state == 1){
        fragColor.rgb = vec3(fragColor.r + (1. - reflectiveness));
    }
    
    /* Metallic */
    if(state == 2){
        fragColor.rgb = vec3((1. - fragColor.r) * metallic);
    }
    
    /* Normal Map */
    if(state == 3){

    }
    
    /* Height */
    if(state == 4){
        fragColor.rgb = vec3(1. - fragColor.r);
        fragColor.rgb = mix(vec3(1. - fragColor.r) , vec3(fragColor.r), height);

    }
    
    /* Ambient Occlusion */
    if(state == 5){
        fragColor.rgb = vec3(1. - fragColor.r + 0.4);
        if(fragColor.r > 1.)
            fragColor.r = 1.;
    }
    vec2 tUV;
    
    if(Pos.x > Pos.z)
        tUV = vec2(Pos.x + Pos.y, Pos.z );
    else
        tUV = vec2(Pos.x , Pos.z + Pos.y);
    float procedural = getProcedural(tUV, Pos, proceduralID, proceduralScale, proceduralInverted);

    float alpha = opacity;
    if(proceduralID == -1)
        alpha *= texture(mask, TexCoords).r; 
    else
        alpha *= procedural;  
    
    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}