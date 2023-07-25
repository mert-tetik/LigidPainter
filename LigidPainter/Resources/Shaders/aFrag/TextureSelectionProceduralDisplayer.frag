#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec2 TexCoords;

out vec4 fragColor;

uniform int proceduralID;

void main(){
    fragColor = vec4(vec3(getProcedural(TexCoords, proceduralID)), 1.);
} 