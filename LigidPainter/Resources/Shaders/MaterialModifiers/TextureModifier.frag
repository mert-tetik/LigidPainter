/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec3 fragPos;
in vec2 TexCoords;  
in vec3 Pos;  

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform int proceduralID;
uniform float proceduralScale;
uniform int proceduralInverted;
uniform sampler2D theTexture;
uniform int theTextureProceduralID;
uniform float theTextureProceduralScale;
uniform int theTextureProceduralInverted;

out vec4 fragColor;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,TexCoords);
}

void main(){
    
    float procedural = getProcedural(Pos, theTextureProceduralID, theTextureProceduralScale, theTextureProceduralInverted);
    if(proceduralID == -1)
        fragColor = texture(theTexture,TexCoords);
    else
        fragColor = vec4(vec3(procedural), 1.);
    
    procedural = getProcedural(Pos, proceduralID, proceduralScale, proceduralInverted);

    float alpha = opacity;
    if(proceduralID == -1)
        alpha *= texture(mask, TexCoords).r; 
    else
        alpha *= procedural; 
    
    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}