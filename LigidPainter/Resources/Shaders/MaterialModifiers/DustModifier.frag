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
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;

/* Element property */
uniform float wetness = 1.;
uniform float metallic = 0.;
uniform float height = -0.2;

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform float depthValue;
uniform sampler2D depthTxtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;


/* Fragment Inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output */
out vec4 fragColor;

float hash(in vec3 p)
{ 
    return fract(sin(dot(p.xyz, vec3(12.9898, 78.233, 34.8454))) * 43758.5453123); 
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

//  -------------- SCRATCHES UTILITIES --------------

float scratch(vec3 uv, vec3 seed)
{
    seed.x = floor(sin(seed.x * 51024.0) * 3104.0);
    seed.y = floor(sin(seed.y * 1324.0) * 554.0);
    seed.z = floor(sin(seed.z * 4848.0) * 2411.0);
 
    uv = uv * 2.0 - 1.0;
    
    rotate(uv, vec3(0,1,0), seed.x + seed.y + seed.z);
    
    uv += sin(seed.x + seed.z - seed.y);
    
    uv = clamp(uv * 0.5 + 0.5, 0.0, 1.0);
    
    float s1 = sin(seed.x + uv.y * 3.1415) * scratchesWavyness;
    float s2 = sin(seed.y + uv.y * 3.1415) * scratchesWavyness;
    float s3 = sin(seed.z + uv.z * 3.1415) * scratchesWavyness;
    
    float x = sign(0.01 - abs(uv.x - 0.5 + s3 + s2 + s1));
    return clamp(((1.0 - pow(uv.y, 2.0)) * uv.y) * 2.5 * x, 0.0, 1.0);
}

float layer(vec3 uv, vec3 frequency, vec3 offset, float angle)
{
    rotate(uv, vec3(0,1,0) ,angle);
    uv = uv * frequency + offset;
    return scratch(fract(uv), floor(uv));
}

float scratches(vec3 uv)
{
    uv *= vec3(scratchesScale,scratchesScale.x);
    vec3 frequency = vec3(scratchesBaseFrequency, scratchesBaseFrequency.x);
    float scratches = 0.0;
    for(int i = 0; i < scratchesLayers; ++i)
    {
        float fi = float(i);
    	scratches = max(scratches, layer(uv, frequency, vec3(fi, fi, fi), fi * 3145.0)); // Nicer blending, thanks Shane!
        frequency += vec3(scratchesFrequencyStep, scratchesFrequencyStep.x);
    }
    return clamp(scratches, 0.0, 1.0); // Saturate for AA to work better
}

//  -------------- NOISE UTILITIES --------------

float noise( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash(i+vec3(0,0,0)), 
                        hash(i+vec3(1,0,0)),f.x),
                   mix( hash(i+vec3(0,1,0)), 
                        hash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(i+vec3(0,0,1)), 
                        hash(i+vec3(1,0,1)),f.x),
                   mix( hash(i+vec3(0,1,1)), 
                        hash(i+vec3(1,1,1)),f.x),f.y),f.z);
}

float fbm(in vec3 p)
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

vec4 mainNoise(vec3 uv)
{
    int samples = blurSamples;
    if(blurIntensity == 0)
        samples = 2;

    vec4 blurredColor = vec4(1.0);  
    for (int i=0; i<=samples/2; i++)
    {
        vec3 mUV = uv - float(i) * (blurIntensity) / float(samples/2) * vec3(blurDirection,0.);
        vec3 pUV = uv + float(i) * (blurIntensity) / float(samples/2) * vec3(blurDirection,0.);
        
        blurredColor += vec4( fbm(pUV + (fbm(pUV) * offsetIntensity) ) ) / vec4(samples);
        blurredColor += vec4( fbm(mUV + (fbm(mUV) * offsetIntensity) ) ) / vec4(samples);

    }
    
    return blurredColor;
    
}


//  -------------- DROPLETS UTILITIES --------------


float dotNoise2D(in float x, in float y, in float z, in float fractionalMaxDotSize, in float dDensity)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;
    
    float integer_z = z - fract(z);
    float fractional_z = z - integer_z;

    if (hash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z + 1.0)) > dDensity){
        return 0.0;
    }

    float xoffset = (hash(vec3(integer_x, integer_y, integer_z)) -0.5);
    float yoffset = (hash(vec3(integer_x + 1.0, integer_y, integer_z)) - 0.5);
    float zoffset = (hash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z)) - 0.5);
    float dotSize = 0.5 * fractionalMaxDotSize * max(0.25,hash(vec3(integer_x, integer_y+1.0, integer_z + 1.0))); //TODO Change the z axis values

    vec3 truePos = vec3(0.5 + xoffset * (1.0 - 2.0 * dotSize) , 0.5 + yoffset * (1.0 -2.0 * dotSize), 0.5 + zoffset * (1.0 - 2.0 * dotSize));

    float distance = length(truePos - vec3(fractional_x, fractional_y, fractional_z));

    return 1.0 - smoothstep(0.3 * dotSize, 1.0* dotSize, distance);

}

float DotNoise2D(in vec3 coord, in float wavelength, in float fractionalMaxDotSize, in float dDensity)
{
   return dotNoise2D(coord.x/wavelength, coord.y/wavelength, coord.z/wavelength, fractionalMaxDotSize, dDensity);
}






float getDroplets(vec3 uv){
    float splash_speed = 1.;
    float transform = 0.;
    int steps = 10;

    float rain = 0.;
    float base_rate = 6.0 + 3.0 * dropletsCount + 4.0 * (splash_speed - 1.0);
    float base_density = 0.6 * dropletsCount + 0.4 * (splash_speed -1.0);

    float opacityJitter = 1. - dropletsOpacityJitter;

    for(int j = 0; j < steps;j++){
        
        float hash = hash(vec3(float(j),float(j)*1.785, float(j)*3.464));
        float opacity = 1. * hash;
        float opacityGap = opacity - 1.;
        opacity -= opacityGap * opacityJitter;        
        
        int samples = blurSamples;
        if(blurIntensity == 0)
            samples = 2;
        for (int i=0; i<=samples/2; i++)
        {
            vec3 mUV = uv - float(i) * (blurIntensity) / float(samples/2) * vec3(blurDirection, 0.);
            vec3 pUV = uv + float(i) * (blurIntensity) / float(samples/2) * vec3(blurDirection, 0.);
        
            vec3 transformedTexCoord = mUV + vec3(0.25 * float(j), 0.35 * float(j), 0.45 * float(j)) + vec3(transform); 
            
            for(int i = 0; i < 4;i++){
        
                float time_fact = (sin(base_rate + (1.570*float(i))));
                time_fact = smoothstep(0.0,1.0,time_fact);
                rain += (DotNoise2D(transformedTexCoord, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity)) / float(samples);
            }
            
            transformedTexCoord = pUV + vec3(0.25 * float(j), 0.35 * float(j), 0.45 * float(j)) + vec3(transform); 
           
            for(int i = 0; i < 4;i++){
        
                float time_fact = (sin(base_rate + (1.570*float(i))));
                time_fact = smoothstep(0.0,1.0,time_fact);
                rain+=(DotNoise2D(transformedTexCoord.xyz, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity)) / float(samples);
            }

        }
    }
    
    return rain;
}


float getScratches(vec3 uv)
{  
    // using AA by Shane:
    // https://www.shadertoy.com/view/4d3SWf
    const float AA = 4.; // Antialias level. Set to 1 for a standard, aliased scene.
    const int AA2 = int(AA*AA);
    float col = 0.0;
    vec3 pix = 2.0/vec3(1024)/AA; // or iResolution.xy
    for (int i=0; i<AA2; i++){ 

        float k = float(i);
        vec3 uvOffs = uv + vec3(floor(k/AA), mod(k, AA), 0.) * pix;
        col += scratches(uvOffs);
    }
    
    col /= (AA*AA);
	
	return col;
}


void main()
{
    if(useMeshMask == 1){
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    // Normalize the position
    vec3 normalizedPosition = normalize(Pos);

    vec3 uv = Pos * size;
    
	vec4 f = vec4(0.0);
    
    float noiseParams[6] = float[](1.0, 4.0, 8.0, 16.0, 32.0, 64.0);
    float noiseWeights[6] = float[](0.25, 0.25, 0.025, 0.00125, 0.025, 0.015);
    float noiseAngleOffsets[6] = float[](0.8, 0.7, -0.6, 0.5, 0.4, -0.3);
    vec3 colorValues[6] = vec3[](color1,color2,color3,color2,color1,color3);

    if(state != 0){
        colorValues = vec3[](vec3(1.),vec3(0.),vec3(1.),vec3(0.),vec3(1.),vec3(0.));
    }

    for (int i = 0; i < concreteSamples; i++) {
          float maskValue = noiseWeights[i] * mainNoise(noiseParams[i] * uv).r; 
          f.a += maskValue;
          f.rgb += colorValues[i] * maskValue;
          uv = rotate(uv, vec3(0,1,0), radians(rotation * noiseAngleOffsets[i]));
    }
    

    uv = rotate(uv, vec3(0,1,0), radians(rotation * noiseAngleOffsets[0]));
    float maskValue = noiseWeights[0] * mainNoise(noiseParams[0] * uv).r; 
    
    f.rgb += getDroplets(uv);

    f.rgb += getScratches(uv);

    f.rgba *= vec4(maskValue - 0.3);
    f.rgba *= 8.;
    
	f = brightness * f; //adjust brightness

    if(state == 0)
        f.rgb = mix(color3, f.rgb, f.a);
	
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
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}