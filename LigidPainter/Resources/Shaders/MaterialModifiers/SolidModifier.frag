/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#version 400 core

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform vec3 value;
uniform float depthValue;
uniform sampler2D depthTxtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;

/* Fragment inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;

void main(){
    if(useMeshMask == 1){
        float prim_txtr_res = int(ceil(sqrt(primitiveCount)));
        float prim_height = floor(float(gl_PrimitiveID) / prim_txtr_res);
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(float(gl_PrimitiveID) - (prim_height * prim_txtr_res) , prim_height), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 
    
    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, value, alpha);

    fragColor = vec4(clrResult, 1.);
}