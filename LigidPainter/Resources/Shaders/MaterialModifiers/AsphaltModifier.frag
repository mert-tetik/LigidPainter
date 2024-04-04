/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#version 400 core

/* Colors */
uniform vec3 asphaltColor = vec3(0.25);
uniform vec3 asphaltColor2 = vec3(0.23,0.25,0.27);

/* Dirt */
uniform float dirtScale = 4.;
uniform float dirtStrength = 1.;

/* 2nd Color */
uniform float colorNoiseScale = 5.;
uniform float colorNoiseStrength = 1.;

/* Noise */ 
uniform float noiseStrength = 1.;

/* Element property */
uniform float wetness = 1.;
uniform float metallic = 0.;
uniform float height = 1.0;

/* Perlin Noise Properties*/
uniform int firstOctave = 3;
uniform int octaves = 8;
uniform float persistence = 0.7;

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

/* Fragment inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;


const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(vec3 xyz)
{
    return fract(tan(distance(xyz * PHI, xyz)) * xyz.x *PHI);
}

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

void main()
{
    if(useMeshMask == 1){
        float prim_txtr_res = int(ceil(sqrt(primitiveCount)));
        float prim_height = floor(float(gl_PrimitiveID) / prim_txtr_res);
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(float(gl_PrimitiveID) - (prim_height * prim_txtr_res) , prim_height), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    vec3 aspColor = asphaltColor; 
    vec3 aspColor2 = asphaltColor2; 
    
    if(state != 0){
        if(state == 5){
            aspColor = vec3(0.25);
            aspColor2 = vec3(0.23,0.25,0.27);
        }
        else{
            aspColor = vec3(0.);
            aspColor2 = vec3(1.);
        }
    }
    
    float perlin = getPerlin(Pos * colorNoiseScale);

    vec3 coloredAsphalt = mix(aspColor, aspColor2 , perlin * colorNoiseStrength); 

    float noise = gold_noise(Pos) * noiseStrength;

    vec3 noisedAsphalt = mix(coloredAsphalt, coloredAsphalt/2., noise); 
    
    perlin = getPerlin(Pos * dirtScale);
    
    vec3 dirtyAsphalt = mix(noisedAsphalt / dirtStrength, noisedAsphalt * 2., perlin);
    
    fragColor = vec4(dirtyAsphalt, 1.);
    
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
        fragColor.rgb = vec3((fragColor.r * perlin + 0.8));
    }
    
    
    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;
    
    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}