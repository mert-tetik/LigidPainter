#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 mirroredView;
uniform mat4 projection;

uniform int renderMirrored;

void main() {
    if(renderMirrored == 0){
        gl_Position = projection * view * vec4(aPos, 0.5);
    }
    else{
        gl_Position = projection * mirroredView * vec4(aPos, 0.5);
    }
}