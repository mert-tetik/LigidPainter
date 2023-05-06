#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelMatrix;

uniform int useUV;

void main() {
    vec3 pos;
    if(useUV == 1)
        pos = vec3(aTexCoords,0);
    else
        pos = aPos;

    vec4 tPos = modelMatrix * vec4(pos,1.0);
    gl_Position = projection * view * vec4(tPos.xyz, 0.5);
}