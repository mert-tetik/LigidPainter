#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D texture;

void main(){
    color = texture2D(texture,TexCoords);
}