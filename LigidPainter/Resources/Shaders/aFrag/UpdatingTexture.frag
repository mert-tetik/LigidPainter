/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
in vec4 ProjectedPos;

//The texture that will be updated
uniform sampler2D txtr; 

//Contains the brush strokes
uniform sampler2D paintingTexture;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 

//Selected opacity for painting
uniform float paintingOpacity;

//Selected color for painting
uniform vec3 paintingColor;

//Do depth testing (painting) if set to 1
uniform int doDepthTest;

//Fragment shader output
out vec4 fragColor;

void main(){

    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, ProjectedPos, paintingOpacity, doDepthTest);

    float txtrAlpha = texture(txtr, TexCoords).a; 

    fragColor = vec4(getBrushedTexture(txtr, brushTxtr, TexCoords, paintingColor, brushModeState),txtrAlpha);

}