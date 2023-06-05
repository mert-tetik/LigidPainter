//Is used to render the skybox

#version 330 core

in vec3 Pos;

uniform samplerCube skybox;

out vec4 color;

void main(){
    color = vec4(textureLod(skybox, Pos, 0.0).rgb * 1,1);
}