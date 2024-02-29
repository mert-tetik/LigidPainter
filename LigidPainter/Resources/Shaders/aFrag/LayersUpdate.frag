#version 400 core

in vec2 TexCoords;

layout(location = 0) out vec4 fragColor0;
layout(location = 1) out vec4 fragColor1;
layout(location = 2) out vec4 fragColor2;
layout(location = 3) out vec4 fragColor3;
layout(location = 4) out vec4 fragColor4;
layout(location = 5) out vec4 fragColor5;

/* Src */
uniform sampler2D albedoTxtr;
uniform sampler2D roughnessTxtr;
uniform sampler2D metallicTxtr;
uniform sampler2D normalMapTxtr;
uniform sampler2D heightMapTxtr;
uniform sampler2D ambientOcclusionTxtr;

/* Masks */
uniform sampler2D generalMaskTxtr;
uniform float generalOpacity;

uniform sampler2D albedoMaskTxtr;
uniform float albedoOpacity;
uniform sampler2D roughnessMaskTxtr;
uniform float roughnessOpacity;
uniform sampler2D metallicMaskTxtr;
uniform float metallicOpacity;
uniform sampler2D normalMapMaskTxtr;
uniform float normalMapOpacity;
uniform sampler2D heightMapMaskTxtr;
uniform float heightMapOpacity;
uniform sampler2D ambientOcclusionMaskTxtr;
uniform float ambientOcclusionOpacity;

void outputColor(out vec4 fragClr, sampler2D src, sampler2D mask, float opacity){
    fragClr = texture(src, TexCoords);
    fragClr.a *= texture(mask, TexCoords).r;
    fragClr.a *= opacity;
    fragClr.a *= texture(generalMaskTxtr, TexCoords).r;
    fragClr.a *= generalOpacity;
}

void main(){
    outputColor(fragColor0, albedoTxtr, albedoMaskTxtr, albedoOpacity);
    outputColor(fragColor1, roughnessTxtr, roughnessMaskTxtr, roughnessOpacity);
    outputColor(fragColor2, metallicTxtr, metallicMaskTxtr, metallicOpacity);
    outputColor(fragColor3, normalMapTxtr, normalMapMaskTxtr, normalMapOpacity);
    outputColor(fragColor4, heightMapTxtr, heightMapMaskTxtr, heightMapOpacity);
    outputColor(fragColor5, ambientOcclusionTxtr, ambientOcclusionMaskTxtr, ambientOcclusionOpacity);
}