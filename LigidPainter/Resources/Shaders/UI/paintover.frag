#version 330 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D txtr;

uniform float opacity;
uniform vec2 pos; 
uniform vec2 scale; 

void main(){
    vec4 src = texture(txtr,TexCoords*scale + pos).rgba;
    fragColor = vec4(src.rgb,opacity*src.a);
}