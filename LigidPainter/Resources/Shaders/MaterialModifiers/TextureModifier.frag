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
uniform sampler2D theTexture;
uniform int proceduralID;
uniform float proceduralScale;
uniform int proceduralInverted;

out vec4 fragColor;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,TexCoords);
}

void main(){
    fragColor = texture(theTexture,TexCoords);
    vec2 tUV;
    
    if(Pos.x > Pos.z)
        tUV = vec2(Pos.x + Pos.y, Pos.z );
    else
        tUV = vec2(Pos.x , Pos.z + Pos.y);
    float procedural = getProcedural(tUV, Pos, proceduralID, proceduralScale, proceduralInverted);

    float alpha = opacity;
    if(proceduralID == -1)
        alpha *= texture(mask, TexCoords).r; 
    else
        alpha *= procedural; 
    
    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}