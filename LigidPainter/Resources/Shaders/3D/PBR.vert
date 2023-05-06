#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelMatrix;


out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec3 Tangent;
out vec3 Bitangent;

out vec4 projectedPos;

uniform int getTheResultV;
uniform mat4 orthoProjection;

void main() {
   
    Tangent = aTangent;
    Bitangent = aBitangent;
    Pos = aPos;
    TexCoords = aTexCoords;
    Normal = aNormal;
  
    vec4 tPos = modelMatrix * vec4(aPos,1.);
    projectedPos = projection * view * vec4(tPos.xyz, 0.5); 
    if(getTheResultV == 0)
        gl_Position = projectedPos;
    else
        gl_Position = orthoProjection * vec4(TexCoords,0.5,1);
}