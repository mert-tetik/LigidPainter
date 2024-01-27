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

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

in vec2 TexCoords;
in vec4 ProjectedPos;

//Contains the brush strokes
uniform sampler2D paintingTexture;

uniform vec3 paintingColor;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

// Painting over
uniform sampler2D paintingOverTexture;
uniform int usePaintingOver;
uniform int paintingOverGrayScale;

//Selected opacity for painting
uniform float paintingOpacity;

//Do depth testing (painting) if set to 1
uniform int doDepthTest;

uniform int selectedPaintingModeIndex;

//Fragment shader output
out vec4 fragColor;

uniform int usingMeshSelection;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;

// Function for blending stroke values
void strokeBlendUniColor(
                            float src,          // Source color
                            float srcA,        // Source alpha
                            float dst,          // Destination color
                            out float color     // Output blended color
                        )
{
    // Blend the alpha values of source and destination
    color = dst + (1.0 - dst) * src; // Blending based on alpha values
    
    // Check if the resulting alpha exceeds the source alpha
    if (color > srcA) {
        color = max(dst, srcA);  // Keep the maximum alpha between source and destination
    }
}

bool isPainted  (
                    vec3 uv, //Screen position (projected position) of the vertex
                    sampler2D depthTexture //Model's depth texture (linearized) 
                ) 
{ 
    //3D Model's depth value in the given coordinate (where the painting will be done)
    float modelZ = texture(depthTexture, uv.xy).r;
    float alpha = texture(depthTexture, uv.xy).a;
    
    if(alpha < 0.99)
        return false;

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
    if(selectedPaintingModeIndex != 2 && selectedPaintingModeIndex != 3){
        brushTxtr.a = brushTxtr.r;
    }
    brushTxtr.a *= opacity; 

    // Check if should be painted
    if(testDepth == 1){
        if(!isPainted(modelCoords,depthTexture)){
            brushTxtr = vec4(0);
        }
    }

    if(selectedPaintingModeIndex == 0){
        // Painting over
        vec2 paintOverUV = modelCoords.xy;
        vec4 paintingOverTxtrVal = texture(paintingOverTexture, paintOverUV);

        // Calculate color val
        if(usePaintingOver == 1){
            if(paintingOverGrayScale == 0){
                brushTxtr.rgb = paintingOverTxtrVal.rgb;
                brushTxtr.a *= paintingOverTxtrVal.a;
            }
            else{
                brushTxtr.rgb = paintingColor;
                brushTxtr.a *= paintingOverTxtrVal.r * paintingOverTxtrVal.a;
            }
        }
        else{
            brushTxtr.rgb = paintingColor;
        }
    }

    // Display the painted side on top of the unpainted
    gl_FragDepth = 0.1;
    if(brushTxtr.a < 0.01)
        gl_FragDepth = 0.9;

    return brushTxtr;
}

void main(){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, screenPos, paintingOpacity, doDepthTest);

    float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
    
    if(!selectedPrim && usingMeshSelection == 1){
        brushTxtr = vec4(0.);
    }

    fragColor = brushTxtr;
}