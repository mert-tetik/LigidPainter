#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec2 TexCoords;

out vec4 fragColor;

uniform int proceduralID;
uniform float proceduralScale;
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;

void main(){
    if(proceduralID > 28)
        fragColor = vec4(getProcedural(vec3(TexCoords.x, TexCoords.y, TexCoords.y), proceduralID, proceduralTexture, TexCoords, proceduralScale, proceduralInverted, vec4(0.), vec2(1000)));
    else
        fragColor = vec4(getProcedural(vec3(TexCoords.x, 0., TexCoords.y), proceduralID, proceduralTexture, TexCoords, proceduralScale, proceduralInverted, vec4(0.), vec2(1000)));
} 