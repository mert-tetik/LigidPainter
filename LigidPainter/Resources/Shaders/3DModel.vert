#version 400 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
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
uniform mat4 oneZeroProjection;
uniform mat4 orthoProjection;

uniform int render2D;
uniform int useTransformUniforms;
uniform vec2 scale;
uniform vec3 pos; 

void main() {
    Tangent = aTangent;
    Bitangent = aBitangent;
    TexCoords = aTexCoords;
    Normal = aNormal;
    Pos = aPos;

    if(useTransformUniforms == 0){
        vec4 tPos = modelMatrix * vec4(Pos,1.);
        projectedPos = projection * view * vec4(tPos.xyz, 0.5); 
    }
    else{
        vec3 scaledPos = aPos * vec3(scale,1);
        projectedPos = orthoProjection * vec4(scaledPos + pos, 1.0);
    }
    
    if(render2D == 0){
        gl_Position = projectedPos;
    }
    else
        gl_Position = oneZeroProjection * vec4(TexCoords.xy,1,1);

}