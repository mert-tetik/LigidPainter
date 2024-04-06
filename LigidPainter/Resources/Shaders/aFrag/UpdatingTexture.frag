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
in vec4 ProjectedPos;

//The texture that will be updated
uniform sampler2D txtr; 

//Contains the brush strokes
uniform sampler2D paintingTexture;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 


uniform int usePaintingOver;

uniform float smearTransformStrength;
uniform float smearBlurStrength;

uniform int multiChannelsPaintingMod;
uniform int channelI;
uniform float channelStrength;


//Fragment shader output
out vec4 fragColor;

void main(){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    vec4 brushTxtr = texture(paintingTexture, TexCoords);

    float txtrAlpha = texture(txtr, TexCoords).a; 

    //fragColor = vec4(texture2D(txtr, TexCoords));
    //fragColor.rgb = vec3(1.) - fragColor.rgb;
 
    fragColor = vec4(getBrushedTexture(txtr, brushTxtr, paintingTexture, TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, multiChannelsPaintingMod == 1, channelI, channelStrength, usePaintingOver == 1));
}