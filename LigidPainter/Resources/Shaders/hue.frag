#version 330 core

in vec3 Colors;

out vec4 color;

void main() {
    color = vec4(Colors,1);
}