/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

uniform int redChannel;

float getTextureMask(sampler2D txtr){
    if(redChannel == 1){
        return texture(txtr, TexCoords).r;
    }
    if(redChannel == 0){
        return texture(txtr, TexCoords).a;
    }

    return 0.;
} 

out vec4 fragColor;

void main(){

    fragColor = texture(txtr1, TexCoords);

    if(txtr2Active == 1)
        fragColor = mix(fragColor, texture(txtr2, TexCoords), getTextureMask(txtr2));

    if(txtr3Active == 1)
        fragColor = mix(fragColor, texture(txtr3, TexCoords), getTextureMask(txtr3));

    if(txtr4Active == 1)
        fragColor = mix(fragColor, texture(txtr4, TexCoords), getTextureMask(txtr4));

    if(txtr5Active == 1)
        fragColor = mix(fragColor, texture(txtr5, TexCoords), getTextureMask(txtr5));

    if(txtr6Active == 1)
        fragColor = mix(fragColor, texture(txtr6, TexCoords), getTextureMask(txtr6));

    if(txtr7Active == 1)
        fragColor = mix(fragColor, texture(txtr7, TexCoords), getTextureMask(txtr7));

    if(txtr8Active == 1)
        fragColor = mix(fragColor, texture(txtr8, TexCoords), getTextureMask(txtr8));

}