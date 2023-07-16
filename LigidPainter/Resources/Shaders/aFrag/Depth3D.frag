/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Depth 3D fragment shader.

    Is used to render the 3D model's depth value for the depth comparing in the painting's depth test section

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Include the painting functions (linearizeDepth & far variable)
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

//Fragment shader output
out vec4 color;

void main() {

    //Output the linearized depth value from the fragment shader
    color = vec4(
                    vec3(linearizeDepth(gl_FragCoord.z)/far), //Linearized depth value
                    1.0 //Full opacity
                );
}