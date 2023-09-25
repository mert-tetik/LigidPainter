/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
in vec2 TexCoords;

//Constant max position count value 
const int maxPosSize = 50;

//Positions of the stroke
uniform vec2 positions[maxPosSize];

//How many position value does a single stroke has (can be greater than the maxPosSize (no restrictions in the cpu))
uniform int posCount; 

//Painting resolution value
uniform vec2 paintingRes;

//Monitor resolution value
uniform vec2 videoScale;

//How many frames passed since started painting
uniform int frame; 

//For the smear brush 
uniform vec2 mouseOffset;

uniform sampler2D bgTxtr;

uniform int redChannelOnly;

uniform vec3 rgbClr;

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


float calculateBrushTexture(vec2 pos, vec2 radius,vec2 uv){
   
    //Positioning the brush position
    uv -= (pos/paintingRes);

    //Expand the radius value to match with the actual radius value
    radius *= 2.;
    
    //Move the texture to the cursor
    uv.x += radius.x/paintingRes.x;
    uv.y += radius.y/paintingRes.y;

    //Scale the texture value
    uv.x /= radius.x/paintingRes.x;
    uv.y /= radius.y/paintingRes.y;
    uv.x /= paintingRes.x/paintingRes.y;

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

    float radius = brush.radius * paintingRes.x;
    
    //Calculate the jitter value for the radius
    radius *= (random);
    float radiusGap = radius - brush.radius * paintingRes.x;
    radius -= radiusGap * brush.sizeJitter;

    //Calculate the fade value for the radius
    if(brush.sinWavePattern == 1)
        radius *= 1. - clamp(sin(radians(float(frame)*3.)),0.,1.) * brush.fade;
    else
        radius *= 1. - clamp(frame/100.,0.,1.) * brush.fade;
    
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

float drawSquare(vec2 uv, vec2 position, vec2 scale)
{
    // Determine the pixel position in normalized coordinates
    vec2 pixelPos = uv;

    // Calculate the color based on pixel position
    float color = 0.; // White square color by default
    if (all(lessThan(abs(pixelPos - position), vec2(scale * 0.5))))
    {
        color = 1.; // Red square color if within the square's bounds
    }

    // Output the color
    return color;
}


void main()
{   
    //The monitor ratio
    float ratio = paintingRes.x/paintingRes.y;
 
    //Expand the uv value to match with the monitor ratio
    vec2 uv = vec2(TexCoords.x,TexCoords.y);
    
    //Brush hardness value
    float hardnessV = min(brush.hardness,1.0);

    //Random value
    float random = hash_normalized_float(frame);

    //Brush radius value (interpeted with fade, radius jitter and the frame values)
    float radius = calculateRadiusValue(random);

    //Result of the painting process
    vec4 orgTxtr;
    float fRes;

    orgTxtr = texture(bgTxtr, vec2(uv.x, 1. - uv.y));
    
    if(redChannelOnly == 1)
        fRes = orgTxtr.r;
    else
        fRes = orgTxtr.a;

    //Calculate all the strokes
    for(int i = 0; i < min(posCount, maxPosSize); i++) {
        
        //Random vec2 value generated with the frame + stroke index value
        vec2 random2 = hash_normalized_vec2(vec2(frame + i, frame + i * 3.486453));
        
        //Calculate the position jitter
        vec2 pos = calculatePosValue(random2, positions[i] / videoScale * paintingRes);

        //Get the brush texture value (the brush texture scaled and moved to the cursor (outside of the boundaries are black))
        float txtr = calculateBrushTexture(pos,vec2(radius / ratio,radius),uv);
        
        //Calculate the alpha value on the white color
        float src = float(1);

        //Calculate the opacity jitter
        float opacity = calculateOpacityValue(random2.r);

        //If the texture is not used individually then calculate the circle and calculate the texture value        
        if(brush.individualTexture != 1){
            
            //Calculate the circle
            float dist = length(uv * paintingRes - pos) / radius; // Calculate the distance of the current pixel from the center of the circle
            src *= smoothstep(
                                    1.0, 
                                    hardnessV * max(0.1, 1.0 - (2.0 / (radius))),
                                    dist
                                );
            
            //Add the texture data
            src *= txtr;
        }

        //If the texture is used individually then equate the alpha value to the texture value        
        else{
            //Calculate the square
            src = drawSquare(uv * paintingRes, pos, vec2(radius) * vec2(2., 2.));

            src *= txtr;
        }

        src *= opacity;


        //Blend the stroke value
        strokeBlendUniColor(src, 1., fRes, fRes);

    }
    
    
    vec4 res = vec4(0.,0.,0.,fRes);

    
    if(redChannelOnly == 0){
        //Calculate the mouse offset value  
        res.rg = orgTxtr.rg + (mouseOffset/videoScale) * max(fRes - orgTxtr.a, 0) ;
        res.b = 0;
        //res.rgb *= ;
    }
    
    if(redChannelOnly == 1)
        res.r = res.a;

    outClr.rgb = mix(res.rgb, rgbClr, rgbClr.r);
    outClr.a = res.a;
}