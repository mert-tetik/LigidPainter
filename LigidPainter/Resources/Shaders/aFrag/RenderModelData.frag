/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Depth 3D fragment shader.

    Is used to render the 3D model's depth value for the depth comparing in the painting's depth test section

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

uniform int state; 
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;


void main() {
    gl_FragDepth = gl_FragCoord.z;

    if(state == 0)
        color = vec4(TexCoords, 0., 1.); 
    else if(state == 1)
        color = vec4((Pos + vec3(1.)) / 2., 1.); 
    else if(state == 2)
        color = vec4((Normal + vec3(1.)) / 2., 1.); 
    else if(state == 3)
        color = vec4(Tangent, 1.); 
    else if(state == 4)
        color = vec4(Bitangent, 1.); 
    else if(state == 5)
        color = vec4(ProjectedPos.rgb, 1.); 
    else
        color = vec4(1.,0.,1., 1.); 

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    
    if(!selectedPrim){
        if(face_selection_data.hideUnselected == 1){
            color.rgba = vec4(0.);
            gl_FragDepth = 1.;
        }
    }
}