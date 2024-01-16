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

uniform sampler2D depthTexture;
uniform sampler2D srcTxtr;
uniform float aoOffset;

uniform int usingMeshSelection = 0;
uniform int hideUnselected = 0;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;

const float far = 1.0;
const float near = 0.5;
float linearizeDepth(float depth){
    float linearizedDepth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
    return linearizedDepth;
}

float isPainted(vec3 uv, sampler2D depthTexture){ 
    float modelZ = texture(depthTexture, uv.xy).r;
    float alpha = texture(depthTexture, uv.xy).a;
    
    if(alpha < 0.95)
        return 0.;
    
    return (modelZ - uv.z);
}

void main(){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    float painted = isPainted(screenPos, depthTexture);
    
    color.rgb = vec3(1. - abs(painted * aoOffset * 1000.));

    color.rgb = vec3(max(color.r, texture(srcTxtr, TexCoords).r));
}