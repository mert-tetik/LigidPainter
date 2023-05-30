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

//2D
out float renderTxtr;
uniform int renderTexture;
uniform mat4 orthoProjection;

void main() {
    renderTxtr = renderTexture;

    Tangent = aTangent;
    Bitangent = aBitangent;
    TexCoords = aTexCoords;
    Normal = aNormal;
    Pos = aPos;

    vec4 tPos = modelMatrix * vec4(Pos,1.);
    projectedPos = projection * view * vec4(tPos.xyz, 0.5); 
    
    if(renderTexture == 0)
        gl_Position = projectedPos;
    else
        gl_Position = orthoProjection * vec4(TexCoords.xy,1,1);

}