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

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

in vec2 TexCoords;
in vec4 ProjectedPos;

//Contains the brush strokes
uniform sampler2D paintingTexture;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

//Selected opacity for painting
uniform float paintingOpacity;

//Do depth testing (painting) if set to 1
uniform int doDepthTest;

//Fragment shader output
out vec4 fragColor;

bool isPainted  (
                    vec3 uv, //Screen position (projected position) of the vertex
                    sampler2D depthTexture //Model's depth texture (linearized) 
                ) 
{ 
    //3D Model's depth value in the given coordinate (where the painting will be done)
    float modelZ = texture(depthTexture, uv.xy).r;

    //TODO : Don't divide with far for the better result
    //Return true if the 3D Model's depth value & the screen position values are similiar
    return abs(modelZ - uv.z) < 0.0001;
}

/**
*   Returns the brush value interpreting the paintingTexture (calculate screen pos & depth test)
**/
vec4 getBrushValue(
                    sampler2D paintingTexture, //Painting texture 
                    sampler2D depthTexture, //Model depth texture
                    vec3 modelCoords, //Screen pos of the model 
                    float opacity, //Brush opacity
                    int testDepth
                )
{
    vec4 brushTxtr = texture(paintingTexture, modelCoords.xy);
    brushTxtr.a *= opacity; 

    if(testDepth == 1){
        if(!isPainted(modelCoords,depthTexture))
            brushTxtr = vec4(0);
    }

    return brushTxtr;
}

void main(){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, screenPos, paintingOpacity, doDepthTest);
    
    fragColor = brushTxtr;
}