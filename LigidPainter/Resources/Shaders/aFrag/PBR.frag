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

uniform sampler2D paintingOverTexture;
uniform int usePaintingOver;
uniform int paintingOverGrayScale;
uniform int paintingOverWraping;
uniform int paintingOverDisplayinMode;

uniform float smearTransformStrength;
uniform float smearBlurStrength;

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

uniform float opacity;

//Fragment shader output
out vec4 fragColor;

// Used to display the mirror displayer
uniform vec3 mirrorState = vec3(0.);
uniform vec3 mirrorOffsets = vec3(0.);

uniform int wireframeMode = 0;
uniform int usingMeshSelection = 0;
uniform int meshSelectionEditing = 0;
uniform int hideUnselected = 0;
uniform sampler2D selectedPrimitiveIDS;


void main() {

    bool selectedPrim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID, 0), 0).r > 0.5;

    if(wireframeMode == 1 || (selectedPrim && meshSelectionEditing == 1)){
        fragColor = vec4(0.18, 0.42, 0.64, 1.);
        return;
    }
    
    //Material channels
    vec3 albedo;
    float roughness;
    float metallic;
    vec3 normal;
    float height;
    float ao;

    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    // Brush value (mask) (painting texture) 
    vec4 brushTxtr = texture(paintingTexture, TexCoords);
    
    if(!(brushModeState == 2 || brushModeState == 3)){
        brushTxtr.a = brushTxtr.r;
    }

    //Get Albedo
    if(paintedTxtrStateIndex == 0)
        albedo = getBrushedTexture(albedoTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).rgb;
    else
        albedo = texture(albedoTxtr,TexCoords).rgb;
    

    //Get Roughness
    if(paintedTxtrStateIndex == 1)
        roughness = getBrushedTexture(roughnessTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).r;
    else
        roughness = texture(roughnessTxtr,TexCoords).r;
    

    //Get Metallic
    if(paintedTxtrStateIndex == 2)
        metallic = getBrushedTexture(metallicTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).r;
    else
        metallic = texture(metallicTxtr,TexCoords).r;


    //Get Normal Map
    if(paintedTxtrStateIndex == 3)
        normal = getBrushedTexture(normalMapTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).rgb;
    else
        normal = texture(normalMapTxtr,TexCoords).rgb;
    
    //Get Height
    if(paintedTxtrStateIndex == 4)
        height = getBrushedTexture(heightMapTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).r;
    else
        height = texture(heightMapTxtr,TexCoords).r;
    

    //Get Ambient Occlusion
    if(paintedTxtrStateIndex == 5)
        ao = getBrushedTexture(ambientOcclusionTxtr,brushTxtr,TexCoords, screenPos.xy, paintingColor, paintingOverTexture, brushModeState, usePaintingOver, paintingOverGrayScale, paintingOverWraping, smearTransformStrength, smearBlurStrength).r;
    else
        ao = texture(ambientOcclusionTxtr,TexCoords).r;


    vec3 pbrResult;

    if(displayingMode == 0)
        pbrResult = getPBR(
                                albedo, roughness, metallic, normal, ao, 
                                Pos, Normal, Tangent, Bitangent, 
                                skybox, prefilterMap, viewPos, 1
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

    if(paintingOverDisplayinMode == 1){
        pbrResult = mix(pbrResult, texture(paintingOverTexture, TexCoords).rgb, 0.5 * texture(paintingOverTexture, TexCoords).a);
    }

    fragColor = vec4(
                        pbrResult, 
                        opacity
                    );

    if(!selectedPrim && usingMeshSelection == 1 && meshSelectionEditing == 0){
        if(hideUnselected == 0)
            fragColor.rgb = mix(fragColor.rgb, vec3(0.5), 0.5);
        else
            fragColor.rgba = vec4(0.);
    }

    float mirrorDisplayerLineThickness = 0.005;
    if(mirrorState.x == 1.){
        if(Pos.x + mirrorOffsets.x > -mirrorDisplayerLineThickness && Pos.x + mirrorOffsets.x < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }
    if(mirrorState.y == 1.){
        if(Pos.y + mirrorOffsets.y > -mirrorDisplayerLineThickness && Pos.y + mirrorOffsets.y < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }
    if(mirrorState.z == 1.){
        if(Pos.z + mirrorOffsets.z > -mirrorDisplayerLineThickness && Pos.z + mirrorOffsets.z < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }

}