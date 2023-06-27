#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Painting.frag)

out vec4 color;

void main() {
    color = vec4(vec3(linearizeDepth(gl_FragCoord.z)/far), 1.0);
}