#version 330 core

in vec3 Pos;

uniform samplerCube skybox;
uniform float exposure;

out vec4 color;

void main(){
    color = vec4(texture(skybox, Pos).rgb * exposure,1);
}