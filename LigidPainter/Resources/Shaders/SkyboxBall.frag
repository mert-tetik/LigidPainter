#version 400 core

struct VertexData{
    vec2 TexCoords;
    vec3 Normal;
    vec3 Pos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 ProjectedPos;
};

in VertexData vertexData;

uniform samplerCube skybox;

out vec4 fragColor;

vec3 getPBR(){
    vec3 N = vertexData.Normal;

    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space

    return texture(skybox, N).rgb ;
}

void main() {
    fragColor = vec4(getPBR(),1.);
}