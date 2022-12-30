#version 330 core

in vec2 TexCoords;


uniform vec3 boxColor;


out vec4 color;

void main() {
    vec3 interpretedColorWhite = ((vec3(1.0)-boxColor) * vec3(TexCoords.x)) + boxColor;
    vec3 interpretedColorBlack = vec3(TexCoords.y)*interpretedColorWhite;
    color = vec4(interpretedColorBlack,1);
}