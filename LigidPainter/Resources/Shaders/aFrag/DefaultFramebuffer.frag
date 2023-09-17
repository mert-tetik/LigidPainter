#version 400 core

in vec2 TexCoords;

uniform sampler2DMS  txtr;
uniform vec2 resolution;

out vec4 fragColor;

void main(){
    fragColor = texelFetch(txtr, ivec2(TexCoords * resolution), 0);
}