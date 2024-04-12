/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using hash functions

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Face_Selection.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/

struct FaceSelectionData{
    int meshSelectionEditing;
    int hideUnselected;
    int usingMeshSelection;
    sampler2D selectedPrimitiveIDS;
    sampler2D meshMask;
    int primitiveCount;
    float prim_txtr_res;
};

float face_selection_get_prim_value(FaceSelectionData face_selection_data){
    float prim_height = floor(float(gl_PrimitiveID) / face_selection_data.prim_txtr_res); // 56
    ivec2 uv = ivec2(float(gl_PrimitiveID) - (prim_height * face_selection_data.prim_txtr_res), prim_height);
    if(uv.x < 0)
        uv.x = 0;
    if(uv.y < 0)
        uv.y = 0;
    if(uv.x >= int(face_selection_data. prim_txtr_res))
        uv.x = int(face_selection_data. prim_txtr_res)-1;
    if(uv.y >= int(face_selection_data. prim_txtr_res))
        uv.y = int(face_selection_data. prim_txtr_res)-1;
    float prim = texelFetch(face_selection_data.selectedPrimitiveIDS, uv, 0).r;
    return prim;

}

bool face_selection_is_current_prim_selected(FaceSelectionData face_selection_data, vec2 texCoords){
    if(face_selection_data.usingMeshSelection == 1 || face_selection_data.meshSelectionEditing == 1){
        float prim = face_selection_get_prim_value(face_selection_data);
        return prim > 0.9 && texture2D(face_selection_data.meshMask, texCoords).r > 0.5;
    }
    else{
        return true;
    }
}

bool face_selection_is_current_prim_hovered(FaceSelectionData face_selection_data, vec2 texCoords){
    if(face_selection_data.usingMeshSelection == 1 || face_selection_data.meshSelectionEditing == 1){
        float prim = face_selection_get_prim_value(face_selection_data);
        return prim > 0.3 && prim < 0.9;
    }
    else{
        return false;
    }
}