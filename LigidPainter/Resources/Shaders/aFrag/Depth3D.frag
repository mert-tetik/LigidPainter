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

//Include the painting functions (linearizeDepth & far variable)
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

//Fragment shader output
out vec4 color;

in vec2 TexCoords;

void main() {

    gl_FragDepth = gl_FragCoord.z;

    //Output the linearized depth value from the fragment shader
    color = vec4(
                    vec3(gl_FragCoord.z), //Linearized depth value
                    1.0 //Full opacity
                );

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    
    if(!selectedPrim){
        if(face_selection_data.hideUnselected == 1){
            color.rgba = vec4(0.);
            gl_FragDepth = 1.;
        }
    }
}