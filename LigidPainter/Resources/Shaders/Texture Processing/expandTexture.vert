#version 330 core
layout(location = 2) in vec2 aTexCoords;

uniform mat4 projection;

out vec2 TexCoords;

void main() {
   
    TexCoords = aTexCoords;
    gl_Position = projection * vec4(aTexCoords, 0.5, 1);
}