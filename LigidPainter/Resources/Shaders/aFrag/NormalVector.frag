#version 400 core

in vec3 Normal;

out vec4 fragColor;

void main(){
    fragColor = vec4(Normal, 1.);
}