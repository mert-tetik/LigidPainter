/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Used for 2D painting.

    Inspired & some parts from :    
    from https://www.shadertoy.com/view/4s33WB
*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core


//Hash functions to calculate the jitter effect
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Hash.frag)


//Retrieve the texture coordinates from the 2DBox vertex shader
in vec2 texCoords;

//Constant max position count value 
const int maxPosSize = 50;

//Positions of the stroke
uniform vec2 positions[maxPosSize];

//How many position value does a single stroke has (can be greater than the maxPosSize (no restrictions in the cpu))
uniform int posCount; 

//Monitor resolution value
uniform vec2 videoScale;

//How many frames passed since started painting
uniform int frame; 

//For the smear brush 
uniform vec2 mouseOffset;

struct Brush {
    float radius;
    float hardness;
    float sizeJitter;
    float fade;
    int sinWavePattern;
    float rotation;
    float rotationJitter;
    float spacing;
    float scatter;
    float alphaJitter;
    int individualTexture;
    sampler2D txtr;
};

//Retrieve the properties of the brush from the CPU
uniform Brush brush;

//Fraagment shader output
out vec4 outClr;

// Function for blending stroke values
void strokeBlendUniColor(
                            vec4 src,          // Source color
                            float srcA,        // Source alpha
                            vec4 dst,          // Destination color
                            out vec4 color     // Output blended color
                        )
{
    color.rgb = src.rgb;  // Set the output color's RGB values to source RGB values
    
    // Blend the alpha values of source and destination
    color.a = dst.a + (1.0 - dst.a) * src.a; // Blending based on alpha values
    
    // Check if the resulting alpha exceeds the source alpha
    if (color.a > srcA) {
        color.a = max(dst.a, srcA);  // Keep the maximum alpha between source and destination
    }
}


float calculateBrushTexture(vec2 pos,float radius){
   
    //The uv value
    vec2 uv = texCoords;

    //Positioning the brush position
    uv -= (pos/videoScale);

    //Expand the radius value to match with the actual radius value
    radius *= 2.;
    
    //Move the texture to the cursor
    uv.x += radius/videoScale.x/2.;
    uv.y += radius/videoScale.y/2.;

    //Scale the texture value
    uv.x /= radius/videoScale.x;
    uv.y /= radius/videoScale.y;
    uv.x /= videoScale.x/videoScale.y;

    //Calculate the rotation value jitter 
    float random = hash_normalized_float(frame);
    float rotation = brush.rotation * random;
    float rotationGap = rotation - brush.rotation;
    rotation -= rotationGap * brush.rotationJitter;

    //Create the rotation jitter
    mat2 rotMatrix = mat2(cos(radians(rotation)), sin(radians(rotation)), -sin(radians(rotation)), cos(radians(rotation)));
    
    //Rotate the uv value
    uv = rotMatrix * uv;
    
    //Flip in the y axis
    uv.y = 1.0 - uv.y;

    //Return the texture value using the transformed texture coordinates
    return  texture (
                        brush.txtr, 
                        uv
                    ).r;
}

float calculateRadiusValue(float random){

    float radius = brush.radius;
    
    //Calculate the jitter value for the radius
    radius *= (random);
    float radiusGap = radius - brush.radius;
    radius -= radiusGap * brush.sizeJitter;

    //Calculate the fade value for the radius
    if(brush.sinWavePattern == 1)
        radius *= 1. - clamp(sin(radians(float(frame)*3.)),0.,1.);
    else
        radius *= 1. - clamp(frame/100.,0.,1.);
    
    radiusGap = radius - brush.radius;
    
    radius -= radiusGap * brush.fade;

    //Return the result
    return radius;

}

vec2 calculatePosValue(vec2 random2, vec2 pos){
    
    //Calculate the jitter value
    vec2 resPos = pos;
    resPos *= (random2*2.);
    vec2 posGap = resPos - pos;
    resPos -= posGap * brush.scatter;

    //Return the result
    return resPos;

}

float calculateOpacityValue(float random){
    //Calculate the jitter value
    float opacity = random;
    float opacityGap = opacity - 1.;
    opacity -= opacityGap * brush.alphaJitter;

    //Return the result 
    return opacity;
}

void main()
{   
    //The monitor ratio
    float ratio = videoScale.x/videoScale.y;
 
    //Expand the uv value to match with the monitor ratio
    vec2 uv = vec2(texCoords.x*ratio,texCoords.y);
    
    //Brush hardness value
    float hardnessV = min(brush.hardness,1.0);

    //Random value
    float random = hash_normalized_float(frame);

    //Brush radius value (interpeted with fade, radius jitter and the frame values)
    float radius = calculateRadiusValue(random);

    //Result of the painting process
    vec4 fRes;

    //Calculate all the strokes
    for(int i = 0; i < min(posCount, maxPosSize); i++) {
        
        //Random vec2 value generated with the frame + stroke index value
        vec2 random2 = hash_normalized_vec2(vec2(frame + i));
        
        //Calculate the position jitter
        vec2 pos = calculatePosValue(random2, positions[i]);
        //Calculate the ratio
        pos.x *= ratio;
        
        //Get the brush texture value (the brush texture scaled and moved to the cursor (outside of the boundaries are black))
        float txtr = calculateBrushTexture(pos,radius);
        
        //Calculate the alpha value on the white color
        vec4 src = vec4(1);

        //If the texture is not used individually then calculate the circle and calculate the texture value        
        if(brush.individualTexture != 1){
            
            //Calculate the circle
            float dist = length(uv * videoScale - pos) / radius; // Calculate the distance of the current pixel from the center of the circle
            src.a *= smoothstep(
                                    1.0, 
                                    hardnessV * max(0.1, 1.0 - (2.0 / (radius))),
                                    dist
                                );
            
            //Add the inverted texture data
            src.a *= 1. -txtr;
        }

        //If the texture is used individually then equate the alpha value to the texture value        
        else{
            src.a *= txtr;
        }

        //Blend the stroke value
        strokeBlendUniColor(src, 1., fRes, fRes);

    }
    
    //Calculate the opacity jitter
    float opacity = calculateOpacityValue(random);
    
    //Equate the fragment output to the painting result value
    outClr = fRes;

    //Calculate the mouse offset value  
    outClr.rg = mouseOffset/videoScale * 10.;
    outClr.b = 0;
    
    //Calculate the opacity value (painting result)
    outClr.a *= opacity;
    outClr.rgb *= outClr.a;
}