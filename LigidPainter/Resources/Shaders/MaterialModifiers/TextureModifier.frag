#version 330 core

in vec3 fragPos;
in vec2 texCoords;

uniform sampler2D albedoTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D heightMapTexture;
uniform sampler2D ambientOcclusionTexture;

layout(location=0) out vec4 albedo;
layout(location=1) out vec4 roughness;
layout(location=2) out vec4 metallic; 
layout(location=3) out vec4 normalMap;
layout(location=4) out vec4 heightMap;
layout(location=5) out vec4 ambientOcclusion;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,texCoords);
}

void main(){
    getImage(albedoTexture,albedo);
    getImage(roughnessTexture,roughness);
    getImage(metallicTexture,metallic);
    getImage(normalMapTexture,normalMap);
    getImage(heightMapTexture,heightMap);
    getImage(ambientOcclusionTexture,ambientOcclusion);
}