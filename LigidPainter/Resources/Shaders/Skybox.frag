#version 330 core

in vec3 Pos;

uniform samplerCube skybox;

out vec4 color;

void main(){
    color = vec4(texture(skybox, Pos).rgb * 1,1);
}