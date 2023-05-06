#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelMatrix;

void main() {
    vec4 tPos = modelMatrix * vec4(aPos,1.0);
    gl_Position = projection * view * vec4(tPos.xyz, 0.5);
}