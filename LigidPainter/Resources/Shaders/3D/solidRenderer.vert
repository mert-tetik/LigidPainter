#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
//layout(location = 3) in vec3 aTangent;
//layout(location = 4) in vec3 aBitangent;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 mostFar;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
//out vec3 Tangent;
//out vec3 Bitangent;

out vec4 projectedPos;

void main() {
   
    //Tangent = aTangent;
    //Bitangent = aBitangent;
    Pos = aPos;
    TexCoords = aTexCoords;
    Normal = aNormal;
    projectedPos = projection * view * vec4((aPos/ abs(mostFar)), 1.0); 
    gl_Position = projection * view * vec4((aPos/ abs(mostFar)), 1.0);
}