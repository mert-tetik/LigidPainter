//Most of the GUI elements is rendered here
//Such as buttons, range bars, checkbox etc.

#version 330 core

out vec4 fragColor;

in vec2 texCoords;
//in vec3 fragPos;

uniform vec4 color;

void main(){
    fragColor = vec4(color);
}