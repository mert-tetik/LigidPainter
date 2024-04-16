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


float checkerPattern(
                        vec2 p
                    )  
{
    p = 0.5 - fract(p);
    return 0.5 + 0.5*sign(p.x*p.y);
}

vec3 get_channel(sampler2D txtr, bool painting, vec3 checker_val, int channelI){
    vec4 val = vec4(0.);
    
    // Get the value
    if(painting){
        val = getBrushedTexture(painting_data, channelI, TexCoords);
    }
    else
        val = texture(txtr,TexCoords);
    
    // Apply checker texture
    if(val.a != 1.)
        val.rgb = mix(checker_val, val.rgb, val.a);

    return val.rgb;
}

void main() {

    gl_FragDepth = gl_FragCoord.z;

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    bool hoveredPrim = face_selection_is_current_prim_hovered(face_selection_data, TexCoords);

    if(!selectedPrim && face_selection_data.meshSelectionEditing == 0 && face_selection_data.hideUnselected == 1){
        fragColor.rgba = vec4(0.);
        gl_FragDepth = 1.;
        return;
    }
    
    vec3 checker_val = vec3(checkerPattern(Pos.xy * 10.) / 8. + 0.5);
    
    //Material channels
    vec3 albedo = get_channel(painting_data.painting_buffers.albedo_txtr, enableAlbedoChannel == 1 && paintingMode == 1, checker_val, 0);
    vec3 roughness = get_channel(painting_data.painting_buffers.roughness_txtr, enableRoughnessChannel == 1 && paintingMode == 1, checker_val, 1);
    vec3 metallic = get_channel(painting_data.painting_buffers.metallic_txtr, enableMetallicChannel == 1 && paintingMode == 1, checker_val, 2);
    vec3 normal = get_channel(painting_data.painting_buffers.normal_map_txtr, enableNormalMapChannel == 1 && paintingMode == 1, checker_val, 3);
    vec3 height = get_channel(painting_data.painting_buffers.height_map_txtr, enableHeightMapChannel == 1 && paintingMode == 1, checker_val, 4);
    vec3 ao = get_channel(painting_data.painting_buffers.ao_txtr, enableAOChannel == 1 && paintingMode == 1, checker_val, 5);

    vec3 pbrResult;

    if(displayingMode == 0)
        pbrResult = getPBR(
                                albedo.rgb, roughness.r, metallic.r, normal.rgb, ao.r, 
                                Pos, Normal, Tangent, Bitangent, 
                                skybox, prefilterMap, viewPos, 1
                            );
    if(displayingMode == 1)
        pbrResult = albedo;
    if(displayingMode == 2)
        pbrResult = roughness;
    if(displayingMode == 3)
        pbrResult = metallic;
    if(displayingMode == 4)
        pbrResult = normal;
    if(displayingMode == 5)
        pbrResult = height;
    if(displayingMode == 6)
        pbrResult = ao;

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

    fragColor.rgb = mix(vec3(checkerPattern(gl_FragCoord.xy / 5.)), fragColor.rgb, fragColor.a);
}