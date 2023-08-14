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
uniform float depthValue;
uniform sampler2D depthTxtr;

/* Fragment Inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;

float hash(vec3 p) {
    p = fract(p * vec3(5.3983, 5.4427, 5.5191));
    p += dot(p, p + 47.57);
    return fract(p.x * p.y);
}

float noise(in vec3 p)
{
    vec3 i = floor(p);
    vec3 xf = fract(p);

    float a = hash(i);
    float b = hash(i + vec3(1.0, 0.0, 0.0));
    float c = hash(i + vec3(0.0, 1.0, 0.0));
    float d = hash(i + vec3(1.0, 1.0, 0.0));
    float e = hash(i + vec3(0.0, 0.0, 1.0));
    float f = hash(i + vec3(1.0, 0.0, 1.0));
    float g = hash(i + vec3(0.0, 1.0, 1.0));
    float h = hash(i + vec3(1.0, 1.0, 1.0));

    vec3 u = xf * xf * (3.0 - 2.0 * xf);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y +
           (e - a) * u.z * (1.0 - u.x) + (f - b) * u.x * u.z + (g - a) * u.y * u.z +
           (h - a) * u.x * u.y * u.z;
}

float fbm(in vec3 p, int octaves, float roughness)
{
  float value = 0.0;
  float amplitude = 1.0;
  float frequency = 2.;

  for (int i = 0; i < octaves; i++)
  {
    value += amplitude * noise(p);
    p *= 3.;
    amplitude *= roughness;
  }
    
  return value;
}

vec3 fbm3D( in vec3 p )
{
    return vec3( fbm(p.xyz, fbmOctaves, fbmRoughness), fbm(p.yxz, fbmOctaves, fbmRoughness) , fbm(p.zyx, fbmOctaves, fbmRoughness));
}

// Rusty metal texture mapping
float mapRustyMetalTexture(vec3 p) {   
    // Combining multiple layers of FBM
    vec3 fbm_0 = 4.0*(p + fbm3D(4.0*p));
    vec3 fbm_1 = p + fbm3D(fbm_0);
    vec3 fbm_2 = fbm3D(fbm_1);
    float f = dot(fbm_2 , vec3(1.0,0.0,1.0) );
    return f;
}

//Not able to use bit operator like <<, so use alternative noise function from YoYo
//
//https://www.shadertoy.com/view/Mls3RS
//
//And it is a better realization I think
float noise(int x, int y, int z)
{
    float fx = float(x);
    float fy = float(y);
    float fz = float(z);
    
    return 2.0 * fract(sin(dot(vec3(fx, fy, fz), vec3(12.9898, 78.233, 45.543))) * 43758.5453) - 1.0;
}

float smoothNoise(int x, int y, int z)
{
    return noise(x, y, z) / 4.0 +
           (noise(x + 1, y, z) + noise(x - 1, y, z) + noise(x, y + 1, z) + noise(x, y - 1, z)) / 8.0 +
           (noise(x + 1, y + 1, z) + noise(x + 1, y - 1, z) + noise(x - 1, y + 1, z) + noise(x - 1, y - 1, z)) / 16.0;
}

float COSInterpolation(float x, float y, float n)
{
    float r = n * 3.1415926;
    float f = (1.0 - cos(r)) * 0.5;
    return x * (1.0 - f) + y * f;
}

float InterpolationNoise(float x, float y, float z)
{
    int ix = int(x);
    int iy = int(y);
    int iz = int(z);
    float fracx = x - float(ix);
    float fracy = y - float(iy);
    float fracz = z - float(iz);
    
    float v1 = smoothNoise(ix, iy, iz);
    float v2 = smoothNoise(ix + 1, iy, iz);
    float v3 = smoothNoise(ix, iy + 1, iz);
    float v4 = smoothNoise(ix + 1, iy + 1, iz);
    float v5 = smoothNoise(ix, iy, iz + 1);
    float v6 = smoothNoise(ix + 1, iy, iz + 1);
    float v7 = smoothNoise(ix, iy + 1, iz + 1);
    float v8 = smoothNoise(ix + 1, iy + 1, iz + 1);
    
    float i1 = COSInterpolation(v1, v2, fracx);
    float i2 = COSInterpolation(v3, v4, fracx);
    float i3 = COSInterpolation(v5, v6, fracx);
    float i4 = COSInterpolation(v7, v8, fracx);
    
    float j1 = COSInterpolation(i1, i2, fracy);
    float j2 = COSInterpolation(i3, i4, fracy);
    
    return COSInterpolation(j1, j2, fracz);
}

float PerlinNoise3D(float x, float y, float z)
{
    float sum = 0.0;
    float frequency = 0.0;
    float amplitude = 0.0;
    for (int i = firstOctave; i < octaves + firstOctave; i++)
    {
        frequency = pow(2.0, float(i));
        amplitude = pow(persistence, float(i));
        sum += InterpolationNoise(x * frequency, y * frequency, z * frequency) * amplitude;
    }
    
    return sum;
}

float getPerlin(vec3 vertexPosition)
{
    float x = vertexPosition.x;
    float y = vertexPosition.y;
    float z = vertexPosition.z;
    float noise = 0.3 + 0.7 * PerlinNoise3D(x, y, z);
    return noise;
}

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

float getWorleyNoise( vec3 uv )
{
    vec4 col = vec4(0.);
    
    float slices = 128.; // number of layers of the 3d texture
    float freq = 4.;
    
    float pfbm= mix(1., perlinfbm(vec3(uv), 4., 7), .5);
    pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise
    
    col.g += worleyFbm(vec3(uv), freq);
    col.b += worleyFbm(vec3(uv), freq*2.);
    col.a += worleyFbm(vec3(uv), freq*4.);
    col.r += remap(pfbm, 0., 1., col.g, 1.); // perlin-worley
    
    return 1. - col.z;
}

// returns closest, second closest, and cell id
float getVoronoi( vec3 x )
{
vec3 p1; vec3 p2;

  vec3 p = floor( x );
  vec3 f = fract( x );

  float id = 0.0;
  vec2 res = vec2( 100.0 );
  
  p1 = vec3(1000.);
  p2 = p1;
  
  for( int k=-1; k<=1; k++ )
    for( int j=-1; j<=1; j++ )
      for( int i=-1; i<=1; i++ )
        {
          // b is cell with integer coordinates
          vec3 b = vec3( float(i), float(j), float(k) );
          vec3 r = vec3( b ) - f + hash( p + b );
          float d = dot( r, r );

          if( d < res.x )
            {
              id = dot( p+b, vec3(1.0,57.0,113.0 ) );
              res = vec2( d, res.x );
              p2 = p1;
              p1 = r;
            }
          else if( d < res.y )
            {
              res.y = d;
              p2 = r;
            }
        }

  return res.r;
}

const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(vec3 xyz)
{
    return fract(tan(distance(xyz * PHI, xyz)) * xyz.x *PHI);
}

vec3 rotate(vec3 v, vec3 axis, float angle)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float oneMinusCos = 1.0 - cosAngle;

    // Normalize the axis vector
    axis = normalize(axis);

    float x = v.x;
    float y = v.y;
    float z = v.z;

    // Apply the rotation formula
    vec3 rotatedVec;
    rotatedVec.x = (cosAngle + axis.x * axis.x * oneMinusCos) * x + (axis.x * axis.y * oneMinusCos - axis.z * sinAngle) * y + (axis.x * axis.z * oneMinusCos + axis.y * sinAngle) * z;
    rotatedVec.y = (axis.y * axis.x * oneMinusCos + axis.z * sinAngle) * x + (cosAngle + axis.y * axis.y * oneMinusCos) * y + (axis.y * axis.z * oneMinusCos - axis.x * sinAngle) * z;
    rotatedVec.z = (axis.z * axis.x * oneMinusCos - axis.y * sinAngle) * x + (axis.z * axis.y * oneMinusCos + axis.x * sinAngle) * y + (cosAngle + axis.z * axis.z * oneMinusCos) * z;

    return rotatedVec;
}
float restriction(float value, float rest){
    if(value < rest)
        value -= rest - value;
    if(value < 0.)
        value = 0.;
    return value;
}

void main() {
    vec3 aColor1 = color1;    
    vec3 aColor2 = color2;    
    vec3 aColor3 = color3;
    vec3 aColor4 = color4;

    if(state != 0){
        aColor1 = vec3(0.0); //Metal
        aColor2 = vec3(0.2); //Damaged Metal
        aColor3 = vec3(0.8); //Brown Color
        aColor4 = vec3(1.0); //Rust Color
    }

    vec3 p = Pos * scale;
    
    float rustyNoise = mapRustyMetalTexture(p) * 1.3;
    float rustyNoise2 = mapRustyMetalTexture(p*4.);
    float rustyNoise3 = mapRustyMetalTexture(rotate(p, vec3(0,1,0), 30.));
    vec3 result = vec3(0);
    float perlin = getPerlin(p)*2.;
    float voronoi = getVoronoi(p/5.);
    float voronoi2 = getVoronoi(p/10.);

    float noise1 = rustyNoise;
    if(noise1 < 0.7)
        noise1 -= 0.7 - noise1;
    
    result = mix(aColor1, aColor2, noise1 * perlin * batteringStrength);
    result = mix(result, aColor2, (voronoi * perlin) / 4.);

    float noise2 = rustyNoise;
    noise2 = restriction(noise2, 0.8);
        
    result = mix(result, aColor3, noise2 * perlin * voronoi * batteringStrength);
   
    float noise3 = rustyNoise;
    noise3 = restriction(noise3, rustRadius);
        
    float noise4 = rustyNoise2;
    noise4 = restriction(noise4, rustRadius);
        
    result = mix(result, aColor4, noise3 * voronoi2);
    result = mix(result, aColor4, noise4);

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
        
    result = mix(result, aColor4, noise5);
    
    voronoi2 = restriction(voronoi2, 0.9);
    
    result = mix(result, aColor3, voronoi2 * perlin * noise3);
    
    result = mix(result, aColor3, voronoi2/4.);

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
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}