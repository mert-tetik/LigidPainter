#version 400 core

in vec2 TexCoords;

uniform sampler2DMS  txtr;
uniform sampler2D  txtr2;
uniform vec2 resolution;
uniform int multisamples;

out vec4 fragColor;

void main(){
    if(multisamples == 1)
        fragColor = texelFetch(txtr, ivec2(TexCoords * resolution), 0);
    else
        fragColor = texture(txtr2, TexCoords);

    fragColor.a = 1.;
}