#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec2 TexCoords;

out vec4 fragColor;

uniform int proceduralID;
uniform float proceduralScale;
uniform float proceduralStretch;
uniform float proceduralScaleModelPos;
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;
uniform int proceduralGrayScale;
uniform float proceduralBrightness;

uniform float displayOpacity;

void main(){
    fragColor = vec4(getProcedural(vec3(TexCoords.x, TexCoords.y, 0.), proceduralID, proceduralTexture, TexCoords, proceduralScale, proceduralStretch, proceduralInverted, vec4(0.), vec2(1000), proceduralGrayScale, proceduralBrightness));

    fragColor.a *= displayOpacity;
} 