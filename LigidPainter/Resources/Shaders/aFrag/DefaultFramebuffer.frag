#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;

out vec4 fragColor;

void main(){
    fragColor = texture(txtr, TexCoords);
}