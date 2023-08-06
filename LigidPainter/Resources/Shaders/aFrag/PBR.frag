#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)


//Functions related to PBR 
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)


in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;


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

//Contains the brush strokes
uniform sampler2D paintingTexture;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

//0 = paint the albedo
//1 = paint the roughness
//2 = paint the metallic 
//3 = paint the normal map
//4 = paint the height map
//5 = paint the ambient Occlusion
uniform int paintedTxtrStateIndex;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 

//Selected opacity for painting
uniform float paintingOpacity;

//Selected color for painting
uniform vec3 paintingColor;

uniform int displayingMode = 0; 

//Fragment shader output
out vec4 fragColor;



void main() {

    //Material channels
    vec3 albedo;
    float roughness;
    float metallic;
    vec3 normal;
    float height;
    float ao;

    // Brush value (mask) (painting texture) 
    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, ProjectedPos, paintingOpacity, 1);
    
    //Get Albedo
    if(paintedTxtrStateIndex == 0)
        albedo = getBrushedTexture(albedoTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).rgb;
    else
        albedo = texture(albedoTxtr,TexCoords).rgb;
    

    //Get Roughness
    if(paintedTxtrStateIndex == 1)
        roughness = getBrushedTexture(roughnessTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).r;
    else
        roughness = texture(roughnessTxtr,TexCoords).r;
    

    //Get Metallic
    if(paintedTxtrStateIndex == 2)
        metallic = getBrushedTexture(metallicTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).r;
    else
        metallic = texture(metallicTxtr,TexCoords).r;


    //Get Normal Map
    if(paintedTxtrStateIndex == 3)
        normal = getBrushedTexture(normalMapTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).rgb;
    else
        normal = texture(normalMapTxtr,TexCoords).rgb;
    
    //Get Height
    if(paintedTxtrStateIndex == 4)
        height = getBrushedTexture(heightMapTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).r;
    else
        height = texture(heightMapTxtr,TexCoords).r;
    

    //Get Ambient Occlusion
    if(paintedTxtrStateIndex == 5)
        ao = getBrushedTexture(ambientOcclusionTxtr,brushTxtr,TexCoords, paintingColor, brushModeState).r;
    else
        ao = texture(ambientOcclusionTxtr,TexCoords).r;


    vec3 pbrResult = getPBR(
                                albedo, roughness, metallic, normal, ao, 
                                Pos, Normal, Tangent, Bitangent, 
                                skybox, prefilterMap, viewPos
                            );

    if(displayingMode == 1)
        pbrResult = albedo;
    if(displayingMode == 2)
        pbrResult = vec3(roughness);
    if(displayingMode == 3)
        pbrResult = vec3(metallic);
    if(displayingMode == 4)
        pbrResult = normal;
    if(displayingMode == 5)
        pbrResult = vec3(height);
    if(displayingMode == 6)
        pbrResult = vec3(ao);

    fragColor = vec4(
                        pbrResult, 
                        1.
                    );
}