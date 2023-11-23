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

in vec3 fragPos;
in vec2 TexCoords;  
in vec3 Pos;  

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform sampler2D theTexture;
uniform float depthValue;
uniform sampler2D depthTxtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;

out vec4 fragColor;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,TexCoords);
}

void main(){
    if(useMeshMask == 1){
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    fragColor = texture(theTexture, TexCoords);
    
    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha * fragColor.a);

    fragColor = vec4(clrResult, 1.);
}