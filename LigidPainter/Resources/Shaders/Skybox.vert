//Is used to render the skybox

#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transformMatrix;

out vec3 Pos;

void main(){
    Pos = aPos;
    vec4 xPos = transformMatrix * vec4(aPos,1);
    vec4 cubePos = projection * mat4(mat3(view))  * vec4(-xPos);
    gl_Position = cubePos.xyww;
}