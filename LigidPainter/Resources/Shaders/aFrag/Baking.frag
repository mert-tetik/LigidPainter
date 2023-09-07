#version 400 core

//Functions related to PBR 
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;

//Position of the camera
uniform vec3 viewPos;

//Used for ambient
//TODO : Don't use the skybox use prefilterMap instead & use blury
uniform samplerCube skybox;

//Used for reflection
uniform samplerCube prefilterMap;

//Material channels
uniform sampler2D albedoTxtr; //Albedo
uniform sampler2D roughnessTxtr; //Roughness
uniform sampler2D metallicTxtr; //Metallic
uniform sampler2D normalMapTxtr; //Normal Map
uniform sampler2D heightMapTxtr; //Ambient occlusion (ao)
uniform sampler2D ambientOcclusionTxtr; //Ambient occlusion (ao)

//Fragment shader output
out vec4 fragColor;

void main() {

    //Material channels
    vec3 albedo = texture(albedoTxtr,TexCoords).rgb;
    float roughness = texture(roughnessTxtr,TexCoords).r;
    float metallic = texture(metallicTxtr,TexCoords).r;
    vec3 normal = texture(normalMapTxtr,TexCoords).rgb;
    float height = texture(heightMapTxtr,TexCoords).r;
    float ao = texture(ambientOcclusionTxtr,TexCoords).r;

    vec3 pbrResult = getPBR(
                            albedo, roughness, metallic, normal, ao, 
                            Pos, Normal, Tangent, Bitangent, 
                            skybox, prefilterMap, viewPos
                        );
    
    fragColor = vec4(
                        pbrResult, 
                        1.
                    );
}