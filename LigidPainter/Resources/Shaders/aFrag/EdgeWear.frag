#version 400 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D normalVectorTxtr;
uniform sampler2D normalVectorTxtrBlurred;
uniform sampler2D noiseTexture;
uniform int invert;
uniform float brightness;
uniform float softness;
uniform float noiseStrength;

vec4 difference(vec4 baseColor, vec4 blendColor) {
    vec4 resultColor;
    const float fac = 1.;
    resultColor = mix(baseColor, abs(baseColor - blendColor), fac);
    resultColor.a = 1.0; // Set the alpha channel to 1.0 (fully opaque)
    return resultColor;
}

void main(){
    fragColor = difference(texture(normalVectorTxtr, TexCoords), texture(normalVectorTxtrBlurred, TexCoords));
    fragColor.r = max(max(fragColor.r, fragColor.g), fragColor.b); 

    if(invert == 1)
        fragColor.r = 1. - fragColor.r;

    fragColor.r *= softness;  

    float noise = texture(noiseTexture, TexCoords).r;

    float orgFragColor = fragColor.r;

    if(fragColor.r < 1. && fragColor.r > 0.){
        if(fragColor.r > 0.5)
            fragColor.r = mix(fragColor.r * noise, fragColor.r, fragColor.r);
        else
            fragColor.r *= noise;
    }

    fragColor.r = mix(orgFragColor, fragColor.r, noiseStrength) * brightness;
    
    fragColor.g = fragColor.r; 
    fragColor.b = fragColor.r;
}
