/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Used for rendering consecutive dozens of tiny dots.

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr1;
uniform int txtr2Active;
uniform sampler2D txtr2;
uniform int txtr3Active;
uniform sampler2D txtr3;
uniform int txtr4Active;
uniform sampler2D txtr4;
uniform int txtr5Active;
uniform sampler2D txtr5;
uniform int txtr6Active;
uniform sampler2D txtr6;
uniform int txtr7Active;
uniform sampler2D txtr7;
uniform int txtr8Active;
uniform sampler2D txtr8;

out vec4 fragColor;

void main(){

    fragColor = texture(txtr1, TexCoords);

    if(txtr2Active == 1)
        fragColor = mix(fragColor, texture(txtr2, TexCoords), texture(txtr2, TexCoords).r);

    if(txtr3Active == 1)
        fragColor = mix(fragColor, texture(txtr3, TexCoords), texture(txtr3, TexCoords).r);

    if(txtr4Active == 1)
        fragColor = mix(fragColor, texture(txtr4, TexCoords), texture(txtr4, TexCoords).r);

    if(txtr5Active == 1)
        fragColor = mix(fragColor, texture(txtr5, TexCoords), texture(txtr5, TexCoords).r);

    if(txtr6Active == 1)
        fragColor = mix(fragColor, texture(txtr6, TexCoords), texture(txtr6, TexCoords).r);

    if(txtr7Active == 1)
        fragColor = mix(fragColor, texture(txtr7, TexCoords), texture(txtr7, TexCoords).r);

    if(txtr8Active == 1)
        fragColor = mix(fragColor, texture(txtr8, TexCoords), texture(txtr8, TexCoords).r);

}