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

in vec2 TexCoords;

uniform sampler2D txtr;

out vec4 fragColor;

void main(){
    fragColor = texture(txtr, TexCoords);
    fragColor.a = 1.;
}