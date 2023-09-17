#version 400 core

in vec2 TexCoords;

uniform sampler2DMS  txtr;

out vec4 fragColor;

void main(){
    fragColor = texelFetch(txtr, ivec2(TexCoords * vec2(1920, 1080)), 0);
}