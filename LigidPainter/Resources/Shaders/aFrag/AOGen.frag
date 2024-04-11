/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

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

//Fragment shader output
out vec4 color;

in vec2 TexCoords;
in vec3 Pos;
in vec4 ProjectedPos;

uniform sampler2D depthTexture;
uniform sampler2D srcTxtr;
uniform float aoOffset;

float isPainted(vec3 uv, sampler2D depthTexture){ 
    float modelZ = texture(depthTexture, uv.xy).r;
    float alpha = texture(depthTexture, uv.xy).a;
    
    if(alpha < 0.95)
        return 0.;
    
    return (modelZ - uv.z);
}

void main(){

    color.a = 1.;

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);

    if(!selectedPrim){
        color = vec4(0.);
        return;
    }

    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    float painted = isPainted(screenPos, depthTexture);
    
    color.rgb = vec3(1. - abs(painted * aoOffset * 1000.));

    color.rgb = vec3(max(color.r, texture(srcTxtr, TexCoords).r));
}