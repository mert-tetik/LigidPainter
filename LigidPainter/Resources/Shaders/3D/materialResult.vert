#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 ortprojection;

uniform int render2D;

out vec2 TexCoords;

void main() {
   
    TexCoords = aTexCoords;
    if(render2D == 0)
        gl_Position = projection * view * vec4(aPos, 0.5);
    else
        gl_Position = ortprojection * vec4(aPos, 1);
}