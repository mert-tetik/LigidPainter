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

uniform int usePaintingOver;

uniform float smearTransformStrength;
uniform float smearBlurStrength;

uniform int paintingMode;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 


uniform int displayingMode = 0; 

uniform float opacity;

//Fragment shader output
out vec4 fragColor;

// Used to display the mirror displayer
uniform vec3 mirrorState = vec3(0.);
uniform vec3 mirrorOffsets = vec3(0.);

uniform int wireframeMode = 0;
uniform int meshSelectionEditing = 0;
uniform int hideUnselected = 0;
uniform int usingMeshSelection = 0;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;

uniform int enableAlbedoChannel;
uniform int enableRoughnessChannel;
uniform float roughnessVal;
uniform int enableMetallicChannel;
uniform float metallicVal;
uniform int enableNormalMapChannel;
uniform float normalMapStrengthVal;
uniform int enableHeightMapChannel;
uniform float heightMapVal;
uniform int enableAOChannel;
uniform float ambientOcclusionVal;

uniform int primitiveCount;

uniform int albedo_only;

void main() {

    gl_FragDepth = gl_FragCoord.z;

    float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;

    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
    bool hoveredPrim = prim > 0.3 && prim < 0.9;

    if(wireframeMode == 1){
        fragColor = vec4(0.18, 0.42, 0.64, 1.);
        return;
    }

    if(!selectedPrim && usingMeshSelection == 1 && meshSelectionEditing == 0 && hideUnselected == 1){
        fragColor.rgba = vec4(0.);
        gl_FragDepth = 1.;
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

    //Get Albedo
    if(((albedo_only == 1) || (albedo_only == 0 && enableAlbedoChannel == 1)) && paintingMode == 1)
        albedo = getBrushedTexture(albedoTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 0, 1., usePaintingOver == 1).rgb;
    else
        albedo = texture(albedoTxtr,TexCoords).rgb;
    

    //Get Roughness
    if(((albedo_only == 0 && enableRoughnessChannel == 1)) && paintingMode == 1){
        roughness = getBrushedTexture(roughnessTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 1, roughnessVal, usePaintingOver == 1).r;
    }
    else
        roughness = texture(roughnessTxtr,TexCoords).r;
    

    //Get Metallic
    if(((albedo_only == 0 && enableMetallicChannel == 1)) && paintingMode == 1){
        metallic = getBrushedTexture(metallicTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 2, metallicVal, usePaintingOver == 1).r;
    }
    else
        metallic = texture(metallicTxtr,TexCoords).r;


    //Get Normal Map
    if(((albedo_only == 0 && enableNormalMapChannel == 1)) && paintingMode == 1){  
        normal = getBrushedTexture(normalMapTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 3, normalMapStrengthVal, usePaintingOver == 1).rgb;
    }
    else
        normal = texture(normalMapTxtr,TexCoords).rgb;
    
    //Get Height
    if(((albedo_only == 0 && enableHeightMapChannel == 1)) && paintingMode == 1){
        height = getBrushedTexture(heightMapTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 4, heightMapVal, usePaintingOver == 1).r;
    }
    else
        height = texture(heightMapTxtr,TexCoords).r;
    

    //Get Ambient Occlusion
    if(((albedo_only == 0 && enableAOChannel == 1)) && paintingMode == 1){
        ao = getBrushedTexture(ambientOcclusionTxtr,brushTxtr,paintingTexture,TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, albedo_only == 0, 5, ambientOcclusionVal, usePaintingOver == 1).r;
    }
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

    fragColor = vec4(
                        pbrResult, 
                        opacity
                    );
    
    if(!selectedPrim && usingMeshSelection == 1 && meshSelectionEditing == 0){
        fragColor.rgb = mix(fragColor.rgb, vec3(1.), 0.6);
    }

    if(selectedPrim && meshSelectionEditing == 1){
        fragColor.rgb = mix(fragColor.rgb, vec3(0.18, 0.42, 0.64), 0.6);
    }

    if(hoveredPrim && meshSelectionEditing == 1)
        fragColor.rgb /= 1.25;

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