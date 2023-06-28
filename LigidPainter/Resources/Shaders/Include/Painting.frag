/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using painting

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/


//----------------------- DEPTH TESTING -----------------------

// Constants
const float far = 100.0;
const float near = 0.1;

/**
 * Linearize the depth value from a depth buffer.
 * 
 * @return The linearized depth value.
 */
float linearizeDepth(
                        float depth //The depth value to linearize.
                    ) 
{

    // Compute the linearized depth value
    float linearizedDepth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));

    return linearizedDepth;

}


bool isPainted  (
                    vec3 uv, //Screen position (projected position) of the vertex
                    sampler2D depthTexture //Model's depth texture (linearized) 
                ) 
{ 

    //3D Model's depth value in the given coordinate (where the painting will be done)
    float modelZ = texture(depthTexture, uv.xy).b;

    //TODO : Don't divide with far for the better result
    //Return true if the 3D Model's depth value & the screen position values are similiar
    return abs(modelZ - linearizeDepth(uv.z)/far) < 0.01;
}


//----------------------- SOFTEN BRUSH ----------------------- 

const int LOD = 1,         // gaussian done on MIPmap at scale LOD
          sLOD = 1 << LOD; // tile size = 2^LOD

/**
* Util function for the getSoftenedTexture function
* Calculate the value of a Gaussian function at the given position.
* 
* returns The computed value of the Gaussian function.
*/
float gaussian(
                vec2 i, //The input position in 2D space 
                int range //The range parameter for the Gaussian function
            ) 
{
    // Compute the standard deviation based on the range parameter.
    float sigma = float(range) * 0.25;

    // Calculate the exponent term of the Gaussian function.
    float exponent = -0.5 * dot(i /= sigma, i);

    // Compute the final value of the Gaussian function.
    float value = exp(exponent) / (6.28 * sigma * sigma);

    return value;
}

/**
*    Apply soften brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSoftenedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getSoftenedTexture(
                            sampler2D txtr, //The texture that will be painted
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords //The texture coordinates
                        )
{ //Intensity from the painted texture
    const float width = 1024;
    int range = 20;
    
    vec4 O = vec4(0); 
    int s = range/sLOD;
    
    // Loop through the subdivisions and accumulate the blurred color
    for (int i = 0; i < s * s; i++) {
        // Compute the offset from the center of the range
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(range) / 2.0;

        // Compute the contribution using the Gaussian function and textureLod
        O += gaussian(d, range) * textureLod(txtr, TexCoords + vec2(1.0 / width) * d, float(LOD));
    }
    
    //Mix the blurred color data with original texture data with the brush texture's alpha value
    return mix(
                texture(txtr,TexCoords).rgb, //Original value
                O.rgb/O.a, //Blurred value
                brushTxtr.a //The brush value
            );
}


//----------------------- SMEAR BRUSH -----------------------


/**
*    Apply smear brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSmearedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getSmearedTexture(
                        sampler2D txtr, //The texture that will be painted  
                        vec4 brushTxtr, //Brush value (painted texture value)
                        vec2 TexCoords  //The texture coordinates
                    )
{
    
    
    float intensity = brushTxtr.a;
    
    //Get the direction value from the painted texture (which direction the painting made in the current texture coordinates)
    vec2 Direction = brushTxtr.rg / 20.;

    //Samples (multiple of 2)
    const int Samples = 64*4;
     
    //Get the blurred color (directional)
    vec4 blurredColor = vec4(0.0);  
    for (int i=0; i<=Samples/2; i++)
    {
        vec2 mUV = TexCoords - float(i) * (intensity) / float(Samples/2) * Direction;
        vec2 pUV = TexCoords + float(i) * (intensity) / float(Samples/2) * Direction;
        
        blurredColor += texture(txtr,pUV);
        blurredColor += texture(txtr,mUV);
    }
    
    blurredColor = blurredColor / float(Samples);    
    
    //TODO : Write smt better to that returning value

    //If painted
    if(intensity > 0.05)
        return blurredColor.rgb;
    
    //If not
    else
        return texture(txtr,TexCoords).rgb;

}


//----------------------- NORMAL BRUSH -----------------------

/**
*    Apply default brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getPaintedTexture(txtr, brushTxtr, TexCoords, vec3(1,0,0)); //Paint to red
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getPaintedTexture  ( 
                            sampler2D txtr, //The texture that will be painted  
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords,  //The texture coordinates
                            vec3 paintingColor //Painting color
                        )
{
    float intensity = brushTxtr.a;

    //Mix the original color data with painting color using the intensity of the painted texture
    return mix  (
                    texture(txtr,TexCoords).rgb,
                    paintingColor,
                    intensity
                );
}


//----------------------- COMBINATION -----------------------  

/**
*    Apply a brush to the given texture
*
*    Use that function as the given examples of the other painting functions 
*
*    brushModeState == 0 : paint with color
*    
*    brushModeState == 1 : paint with softening
*
*    brushModeState == 2 : paint with smearing
*
*    returns the painted color data with given texture coordinate
**/
vec3 getBrushedTexture (
                            sampler2D txtr, //The texture that will be painted  
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords,  //The texture coordinates
                            vec3 paintingColor, //Painting color (used for painting with color (if brushModeState == 0))
                            int brushModeState //Decide which painting mode will be used
                        )
{
    //Apply painting with color
    if(brushModeState == 0)
        return getPaintedTexture(txtr,brushTxtr,TexCoords,paintingColor);

    //Apply painting with softening
    if(brushModeState == 1)
        return getSoftenedTexture(txtr,brushTxtr,TexCoords);

    //Apply painting with smearing
    if(brushModeState == 2)
        return getSmearedTexture(txtr,brushTxtr,TexCoords);

    //If the brushModeState value is not valid
    else
        return vec3(0);
}

/**
*   Returns the brush value interpreting the paintingTexture (calculate screen pos & depth test)
**/
vec4 getBrushValue(
                    sampler2D paintingTexture, //Painting texture 
                    sampler2D depthTexture, //Model depth texture
                    vec4 ProjectedPos, //Screen pos of the model 
                    float opacity //Brush opacity
                )
{
    
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);
    vec4 brushTxtr = texture(paintingTexture, screenPos.xy);
    brushTxtr.a *= opacity; 

    if(!isPainted(screenPos,depthTexture))
        brushTxtr = vec4(0);

    return brushTxtr;
}