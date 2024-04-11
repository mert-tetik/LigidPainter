/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

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

in vec3 fragPos;
in vec2 TexCoords;  
in vec3 Pos;  

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform sampler2D theTexture;
uniform float depthValue;
uniform sampler2D depthTxtr;

out vec4 fragColor;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,TexCoords);
}

void main(){
    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    if(!selectedPrim){
        fragColor = vec4(0.);
        return;
    }

    fragColor = texture(theTexture, TexCoords);
    
    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha * fragColor.a);

    fragColor = vec4(clrResult, 1.);
}