#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec3 Pos;
in vec2 TexCoords;

uniform int proceduralID; 
uniform float proceduralScale; 
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;

out vec4 fragColor;

void main(){
    vec3 procedural = getProcedural(Pos, proceduralID, proceduralTexture, TexCoords, proceduralScale, proceduralInverted);
    fragColor = vec4(vec3(procedural),1.);
}