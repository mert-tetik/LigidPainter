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

out vec4 fragColor;

in vec2 TexCoords;

void main(){
    fragColor = vec4(0.);

    gl_FragDepth = gl_FragCoord.z;
    
    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    
    if(!selectedPrim){
        if(face_selection_data.hideUnselected == 1){
            gl_FragDepth = 1.;
        }
    }
}