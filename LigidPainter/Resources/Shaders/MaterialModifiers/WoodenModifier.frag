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
vec3 woodColor1 = vec3(0.0, 0.0, 0.0);
vec3 woodColor2 = vec3(0.25, 0.11, 0.04);
vec3 woodColor3 = vec3(0.52, 0.32, 0.19);

float scale = 1.;

/* Noise Properties */
float noiseOffset = 1.;
float seed = 3000.;

/* Perlin Properties */
int maxOctaves = 8;
float persistance = 0.5;

/* Musgrave Properties */
float musgraveLacunarity = 2.5;
float musgraveStrength = 0.75;
float musgraveNoise = 1.;

/* Base */
float baseNoiseStrength = 4.6;
float baseColorSaturation = 1.;

/* FBM Properties */
float fbmFrequency = 1.0;

/* Element Properties*/
float woodGamma = 0.6;
float shininess = 1.5;
float metallic = 0.;
float height = 0.0;

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

out vec4 fragColor;


float hash(float n) {
    return fract(sin(n) * seed);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    
    vec3 u = f * f * noiseOffset;
    
    float n000 = hash(dot(i, vec3(1.0, 1.0, 1.0)));
    float n001 = hash(dot(i + vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n010 = hash(dot(i + vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n011 = hash(dot(i + vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n100 = hash(dot(i + vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n101 = hash(dot(i + vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n110 = hash(dot(i + vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n111 = hash(dot(i + vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)));
    
    return mix(
        mix(mix(n000, n100, u.x), mix(n001, n101, u.x), u.y),
        mix(mix(n010, n110, u.x), mix(n011, n111, u.x), u.y),
        u.z
    );
}

float fbm(vec3 p, int octaves, float roughness) {
    
    float freq = fbmFrequency;
    
    float amplitude = 1.0;
    
    float total = 0.0;
    float maxTotal = 0.0;
    
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * noise(p * freq);
        maxTotal += amplitude;
        
        freq *= 2.0;
        amplitude *= roughness;
    }
    
    return total / maxTotal;
}

float fbmx(vec3 p) {
    vec3 noise = vec3(noise(p + vec3(0.)), noise(p + vec3(1.)), noise(p + vec3(2.)));
    p += noise;
    return fbm(p, maxOctaves, persistance);
}

float musgrave(vec3 p, float octaves, float dimension, float lacunarity) {
    float sum = 0.0;
    float amp = 1.0;
    float m = pow(lacunarity, -dimension);
    
    while (octaves-- > 0.0) {
        float n = noise(p) * 2.0 - 1.0;
        sum += n * amp;
        amp *= m * musgraveNoise;
        p *= lacunarity;
    }
    
    return sum;
}

vec3 getWood(vec3 p) {
    
    float n1 = fbmx(p * vec3(8., 1., 1.));
    
    float n2 = mix(musgrave(vec3(n1 * baseNoiseStrength), 8.0, 0.0, musgraveLacunarity), n1 * baseColorSaturation, musgraveStrength);
    
    return mix(mix(woodColor1, woodColor2, n1), woodColor3, n2);
}

void main() {
    //TODO Use the scale
    vec3 woodColor = getWood(Pos);
    
    float gammaVal = 1. - woodGamma;
    vec3 woodGamma = pow(woodColor, vec3(gammaVal));
    
    fragColor = vec4(woodGamma, 0.0);

        /* Roughness */
    if(state == 1){
        fragColor.rgb = vec3(1. - fragColor.r) / shininess;
    }
    
    /* Metallic */
    else if(state == 2){
        fragColor.rgb = vec3(fragColor.r) * metallic;
    }
    
    else if(state == 3){
    
    }
    
    /* Height Map */
    else if(state == 4){
        fragColor.rgb = vec3(fragColor.r/5. + height);
    }

    else if(state == 5){
        fragColor.rgb = vec3(abs(fragColor.r)/ 4. + 0.8);
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    vec3 clrResult = mix(fragColor.rgb, texture(previousTxtr, TexCoords).rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}
