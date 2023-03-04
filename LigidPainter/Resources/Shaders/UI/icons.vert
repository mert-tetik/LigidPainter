#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
uniform mat4 Projection;

uniform vec3 pos;
uniform mat4 scale;

void main(){
    
    TexCoords = aTexCoords;
    
    vec3 scaledPos = vec3(scale * vec4(aPos,1));
    gl_Position = Projection * vec4(scaledPos+pos, 1.0);
}