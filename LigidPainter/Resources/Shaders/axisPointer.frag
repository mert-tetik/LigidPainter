#version 330 core

in vec3 Pos;

out vec4 color;

void main() {

    color = vec4((abs(Pos.x) / 20), (abs(Pos.y) / 20), (abs(Pos.z) / 20), 1);
}