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

//Fragment shader output
out vec4 color;

in vec2 TexCoords;
in vec3 Pos;
in vec4 ProjectedPos;

uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;

uniform sampler2D depthTexture;
uniform sampler2D srcTxtr;
uniform float aoOffset;

float isPainted(vec3 uv, sampler2D depthTexture){ 
    float modelZ = texture(depthTexture, uv.xy).r;
    float alpha = texture(depthTexture, uv.xy).a;
    
    if(alpha < 0.95)
        return 0.;
    
    return (modelZ - uv.z);
}

void main(){

    color.a = 1.;

    if(useMeshMask == 1){
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            color = vec4(0.);
            return;
        }
    }

    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    float painted = isPainted(screenPos, depthTexture);
    
    color.rgb = vec3(1. - abs(painted * aoOffset * 1000.));

    color.rgb = vec3(max(color.r, texture(srcTxtr, TexCoords).r));
}