/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Sources : 

        Scratches : 
        https://www.shadertoy.com/view/4syXRD
    
        Droplets : 
        https://www.shadertoy.com/view/ldfSDj

        Noise :
        https://www.shadertoy.com/view/XtsyRr

*/

#version 400 core

/* Noise */
uniform float size = 2.0; //best 1.0
uniform float offsetIntensity = 5.0; //best 5.0
uniform float rotation = 360.0; //best 360.0
uniform float brightness = 1.0;
int concreteSamples = 6;

/* Blur */
uniform float blurIntensity = 0.;
uniform vec2 blurDirection = vec2(1,0);
int blurSamples = 16;

/* Scratches */
uniform float scratchesWavyness = 0.1;
uniform vec2 scratchesScale = vec2(3.0, 3.0);
int scratchesLayers = 4;
uniform vec2 scratchesBaseFrequency = vec2(0.5, 0.5);
uniform vec2 scratchesFrequencyStep = vec2(0.25, 0.25);

/* Droplets */
uniform float dropletsCount = 0.1;
uniform float dropletsOpacityJitter = 1.;
uniform float dropletsSize = 5.0;

/* Colors */
vec3 color1 = vec3(1.);
vec3 color2 = vec3(0.0);

/* Element property */
float wetness = 1.;
float metallic = 0.;
float height = -0.2;

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

/* Fragment Output */
out vec4 fragColor;


float hash(in vec2 p)
{ 
      return fract(sin(dot(p.xy, vec2(12.9898,78.233))) * 43758.5453123); 
}

float hash2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 rotate(vec2 uv, float a)
{
	return vec2(uv.x*cos(a)-uv.y*sin(a),uv.y*cos(a)+uv.x*sin(a));
}


//  -------------- SCRATCHES UTILITIES --------------

void pR(inout vec2 p, float a)
{
    float sa = sin(a);
    float ca = cos(a);
    p *= mat2(ca, sa, -sa, ca);
}

float scratch(vec2 uv, vec2 seed)
{
    seed.x = floor(sin(seed.x * 51024.0) * 3104.0);
    seed.y = floor(sin(seed.y * 1324.0) * 554.0);
 
    uv = uv * 2.0 - 1.0;
    pR(uv, seed.x + seed.y);
    uv += sin(seed.x - seed.y);
    uv = clamp(uv * 0.5 + 0.5, 0.0, 1.0);
    
    float s1 = sin(seed.x + uv.y * 3.1415) * scratchesWavyness;
    float s2 = sin(seed.y + uv.y * 3.1415) * scratchesWavyness;
    
    float x = sign(0.01 - abs(uv.x - 0.5 + s2 + s1));
    return clamp(((1.0 - pow(uv.y, 2.0)) * uv.y) * 2.5 * x, 0.0, 1.0);
}

float layer(vec2 uv, vec2 frequency, vec2 offset, float angle)
{
    pR(uv, angle);
    uv = uv * frequency + offset;
    return scratch(fract(uv), floor(uv));
}

float scratches(vec2 uv)
{
    uv *= scratchesScale;
    vec2 frequency = scratchesBaseFrequency;
    float scratches = 0.0;
    for(int i = 0; i < scratchesLayers; ++i)
    {
        float fi = float(i);
    	scratches = max(scratches, layer(uv, frequency, vec2(fi, fi), fi * 3145.0)); // Nicer blending, thanks Shane!
        frequency += scratchesFrequencyStep;
    }
    return clamp(scratches, 0.0, 1.0); // Saturate for AA to work better
}

//  -------------- NOISE UTILITIES --------------

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
    vec4 blurredColor = vec4(0.0);  
    for (int i=0; i<=blurSamples/2; i++)
    {
        vec2 mUV = uv - float(i) * (blurIntensity) / float(blurSamples/2) * blurDirection;
        vec2 pUV = uv + float(i) * (blurIntensity) / float(blurSamples/2) * blurDirection;
        
        blurredColor += vec4( fbm(pUV + (fbm(pUV) * offsetIntensity) ) ) / vec4(blurSamples);
        blurredColor += vec4( fbm(mUV + (fbm(mUV) * offsetIntensity) ) ) / vec4(blurSamples);

    }
    
    return blurredColor;
    
}


//  -------------- DROPLETS UTILITIES --------------


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
    int steps = 10;

    float rain = 0.;
    float base_rate = 6.0 + 3.0 * dropletsCount + 4.0 * (splash_speed - 1.0);
    float base_density = 0.6 * dropletsCount + 0.4 * (splash_speed -1.0);

    float opacityJitter = 1. - dropletsOpacityJitter;

    for(int j = 0; j < steps;j++){

        
        float hash = hash(vec2(float(j),float(j)*1.785));
        float opacity = 1. * hash;
        float opacityGap = opacity - 1.;
        opacity -= opacityGap * opacityJitter;        
        
        for (int i=0; i<=blurSamples/2; i++)
        {
            vec2 mUV = uv - float(i) * (blurIntensity) / float(blurSamples/2) * blurDirection;
            vec2 pUV = uv + float(i) * (blurIntensity) / float(blurSamples/2) * blurDirection;
        
            vec2 transformedTexCoord = mUV + vec2(0.25 * float(j), 0.35 * float(j)) + vec2(transform); 
            
            for(int i = 0; i < 4;i++){
        
                float time_fact = (sin(base_rate + (1.570*float(i))));
                time_fact = smoothstep(0.0,1.0,time_fact);
                rain+=(DotNoise2D(transformedTexCoord.xy, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity)) / float(blurSamples);
            }
            
            transformedTexCoord = pUV + vec2(0.25 * float(j), 0.35 * float(j)) + vec2(transform); 
           
            for(int i = 0; i < 4;i++){
        
                float time_fact = (sin(base_rate + (1.570*float(i))));
                time_fact = smoothstep(0.0,1.0,time_fact);
                rain+=(DotNoise2D(transformedTexCoord.xy, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity)) / float(blurSamples);
            }

        }
    }
    
    return rain;
}


float getScratches(vec2 uv)
{  
    // using AA by Shane:
    // https://www.shadertoy.com/view/4d3SWf
    const float AA = 4.; // Antialias level. Set to 1 for a standard, aliased scene.
    const int AA2 = int(AA*AA);
    float col = 0.0;
    vec2 pix = 2.0/vec2(1024)/AA; // or iResolution.xy
    for (int i=0; i<AA2; i++){ 

        float k = float(i);
        vec2 uvOffs = uv + vec2(floor(k/AA), mod(k, AA)) * pix;
        col += scratches(uvOffs);
    }
    col /= (AA*AA);
	
	return col;
}

vec2 getTextureCoordinates(vec3 position, vec3 tangent) {
//   vec3 ANPosition = (abs(position));
  
//   return vec2(ANPosition.x, (ANPosition.y + ANPosition.z) / 2.f);
    
    vec3 bitangent = cross(tangent, position);
  vec3 normal = cross(bitangent, tangent);

  mat3 TBN = mat3(tangent, bitangent, normal);
  vec3 transformedPosition = TBN * position;

  vec2 textureCoord;
  textureCoord.x = 0.5 * transformedPosition.x + 0.5;
  textureCoord.y = 0.5 * transformedPosition.y + 0.5;

  return textureCoord;
}

void main()
{
      // Normalize the position
    vec3 normalizedPosition = normalize(Pos);

    // Scale the normalized position to the range [0, 1]
    vec2 uv = getTextureCoordinates(Pos, Tangent);

    uv *= size;
    
	vec4 f = vec4(0.0);
    
    float noiseParams[6] = float[](1.0, 4.0, 8.0, 16.0, 32.0, 64.0);
    float noiseWeights[6] = float[](0.25, 0.25, 0.025, 0.00125, 0.025, 0.015);
    float noiseAngleOffsets[6] = float[](0.8, 0.7, -0.6, 0.5, 0.4, -0.3);
    vec3 colorValues[6] = vec3[](color1,color2,color1,color2,color1,color2);

    for (int i = 0; i < concreteSamples; i++) {
          float maskValue = noiseWeights[i] * mainNoise(noiseParams[i] * uv).r; 
          f.a += maskValue;
          f.rgb += colorValues[i] * maskValue;
          uv = rotate(uv, radians(rotation * noiseAngleOffsets[i]));
    }
    
    uv = rotate(uv, radians(rotation * noiseAngleOffsets[0]));
    float maskValue = noiseWeights[0] * mainNoise(noiseParams[0] * uv).r; 
    
    f.rgb += getDroplets(uv);

    f.rgb += getScratches(uv);

    f.rgba *= vec4(maskValue - 0.3);
    f.rgba *= 8.;
    
	f = brightness * f; //adjust brightness

	fragColor = vec4(vec3(f.rgb), 1.0);

    /*Roughness*/
    if(state == 1){
        fragColor.rgb = vec3(1. - fragColor.r * wetness);
    }
    
    /*Metallic*/
    if(state == 2){
        fragColor.rgb = vec3(fragColor.r * metallic);
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
        fragColor.rgb = vec3((fragColor.r * 1.5));
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    vec3 clrResult = mix(fragColor.rgb, texture(previousTxtr, TexCoords).rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}