#version 330 core

in vec3 fragPos;
in vec2 texCoords;

uniform sampler2D theTexture;

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
    getImage(theTexture,albedo);
    getImage(theTexture,roughness);
    getImage(theTexture,metallic);
    getImage(theTexture,normalMap);
    getImage(theTexture,heightMap);
    getImage(theTexture,ambientOcclusion);
}