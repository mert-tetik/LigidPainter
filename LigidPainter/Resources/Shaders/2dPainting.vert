#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 color;

uniform mat4 renderProjection;
uniform vec3 transform[500];

void main(){  
    color = aColor;
    texCoords = aTexCoords;

    gl_Position = renderProjection * vec4(vec3(aPos.xy + transform[gl_InstanceID].xy , 0.5), 1.0);
}