#version 400 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D normalVectorTxtr;
uniform sampler2D normalVectorTxtrBlurred;

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
    fragColor.g = fragColor.r; 
    fragColor.b = fragColor.r; 
}
