#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 Pos;

uniform mat4 projection;
uniform mat4 view;

void main(){
    Pos = aPos;
    gl_Position = projection * view * vec4(Pos,1.0);
}