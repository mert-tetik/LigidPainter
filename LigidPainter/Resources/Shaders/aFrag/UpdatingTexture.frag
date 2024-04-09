/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Updating texture fragment shader

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

in vec2 TexCoords;

uniform PaintingData painting_data;
uniform int channel_index;

//Fragment shader output
out vec4 fragColor;

void main(){
    fragColor = vec4(getBrushedTexture(painting_data, channel_index, TexCoords));
}