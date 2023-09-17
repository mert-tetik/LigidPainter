#version 400 core

in vec2 TexCoords;

uniform sampler2D  txtr;
uniform vec2 resolution;

out vec4 fragColor;

void main(){
    fragColor = texture(txtr, TexCoords);

    fragColor.a = 1.;
}