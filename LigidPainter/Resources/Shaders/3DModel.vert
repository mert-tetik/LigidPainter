#version 400 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelMatrix;


struct VertexData{
    vec2 TexCoords;
    vec3 Normal;
    vec3 Pos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 ProjectedPos;
};

out VertexData vertexData;

//2D
uniform mat4 oneZeroProjection;
uniform mat4 orthoProjection;

uniform int render2D;
uniform int useTransformUniforms;
uniform vec2 scale;
uniform vec3 pos; 

void main() {
    vertexData.Tangent = aTangent;
    vertexData.Bitangent = aBitangent;
    vertexData.TexCoords = aTexCoords;
    vertexData.Normal = aNormal;
    vertexData.Pos = aPos;

    if(useTransformUniforms == 0){
        vec4 tPos = modelMatrix * vec4(vertexData.Pos,1.);
        vertexData.ProjectedPos = projection * view * vec4(tPos.xyz, 0.5); 
    }
    else{
        vec3 scaledPos = aPos * vec3(scale,1);
        vertexData.ProjectedPos = orthoProjection * vec4(scaledPos + pos, 1.0);
    }
    
    if(render2D == 0){
        gl_Position = vertexData.ProjectedPos;
    }
    else
        gl_Position = oneZeroProjection * vec4(vertexData.TexCoords.xy,1,1);

}