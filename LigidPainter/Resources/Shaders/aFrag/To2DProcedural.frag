#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec3 Pos;

uniform int proceduralID; 
uniform float proceduralScale; 
uniform int proceduralInverted;

out vec4 fragColor;

void main(){
    float procedural = getProcedural(Pos, proceduralID, proceduralScale, proceduralInverted);
    fragColor = vec4(vec3(procedural),1.);
}