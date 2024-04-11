#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

//Functions related to PBR 
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)


/*
struct FaceSelectionData{
    int meshSelectionEditing;
    int hideUnselected;
    int usingMeshSelection;
    sampler2D selectedPrimitiveIDS;
    sampler2D meshMask;
    int primitiveCount;
};
face_selection_is_current_prim_selected or face_selection_is_current_prim_hovered
*/
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Face_Selection.frag)
uniform FaceSelectionData face_selection_data;

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

uniform int paintingMode;

uniform int displayingMode = 0; 

uniform float opacity;

//Fragment shader output
out vec4 fragColor;

// Used to display the mirror displayer
uniform vec3 mirrorState = vec3(0.);
uniform vec3 mirrorOffsets = vec3(0.);

uniform int enableAlbedoChannel;
uniform int enableRoughnessChannel;
uniform int enableMetallicChannel;
uniform int enableNormalMapChannel;
uniform int enableHeightMapChannel;
uniform int enableAOChannel;


uniform PaintingData painting_data;


void main() {

    gl_FragDepth = gl_FragCoord.z;

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    bool hoveredPrim = face_selection_is_current_prim_hovered(face_selection_data, TexCoords);

    if(!selectedPrim && face_selection_data.meshSelectionEditing == 0 && face_selection_data.hideUnselected == 1){
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

    //Get Albedo
    if(enableAlbedoChannel == 1 && paintingMode == 1){
        albedo = getBrushedTexture(painting_data, 0, TexCoords).rgb;
    }
    else
        albedo = texture(albedoTxtr,TexCoords).rgb;
    

    //Get Roughness
    if(enableRoughnessChannel == 1 && paintingMode == 1){
        roughness = getBrushedTexture(painting_data, 1, TexCoords).r;
    }
    else
        roughness = texture(roughnessTxtr,TexCoords).r;
    

    //Get Metallic
    if(enableMetallicChannel == 1 && paintingMode == 1){
        metallic = getBrushedTexture(painting_data, 2, TexCoords).r;
    }
    else
        metallic = texture(metallicTxtr,TexCoords).r;


    //Get Normal Map
    if(enableNormalMapChannel == 1 && paintingMode == 1){
        normal = getBrushedTexture(painting_data, 3, TexCoords).rgb;
    }
    else
        normal = texture(normalMapTxtr,TexCoords).rgb;
    
    //Get Height
    if(enableHeightMapChannel == 1 && paintingMode == 1){
        height = getBrushedTexture(painting_data, 4, TexCoords).r;
    }
    else
        height = texture(heightMapTxtr,TexCoords).r;
    

    //Get Ambient Occlusion
    if(enableAOChannel == 1 && paintingMode == 1){
        ao = getBrushedTexture(painting_data, 5, TexCoords).r;
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
    
    if(!selectedPrim && face_selection_data.usingMeshSelection == 1 && face_selection_data.meshSelectionEditing == 0){
        fragColor.rgb = mix(fragColor.rgb, vec3(1.), 0.6);
    }

    if(selectedPrim && face_selection_data.meshSelectionEditing == 1){
        fragColor.rgb = mix(fragColor.rgb, vec3(0.18, 0.42, 0.64), 0.6);
    }

    if(hoveredPrim && face_selection_data.meshSelectionEditing == 1)
        fragColor.rgb /= 2.;

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