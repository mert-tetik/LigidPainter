/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Button fragment shader

    Most of the GUI elements are rendered using this shader

    Such as buttons, range bars, checkboxes, panels etc.
*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Texture coordinate data from the vertex shader
in vec2 TexCoords;
//Scale value of the box (GUI element)
in vec2 uniScale;

struct States{
    //Text rendering mode (render ascii characters in the box)
    int renderText;

    //Texture rendering mode (render textures in the box)
    int renderTexture;
};

struct Outline{
    //0 : No outline , 1 : Only outline , 2 : Additional outline
    int state;

    //Color of the outline (if the properties.outline.state set to 2 (otherwise the outline uses the color variable for it's color))
    vec3 color; 

    //Animation color of the outline (if the properties.outline.state set to 2 (otherwise the outline uses the color variable for it's color))
    vec3 color2;

    //Outline thickness value 
    float thickness;
};

//Button properties
struct Properties{
    //Outline of the box
    Outline outline;    

    //Color of the box
    vec4 color;

    //Animation color of the box
    vec4 color2;
    
    //Animation value in the range of 0 - 1 changes gradually to perform an animation
    float colorMixVal;

    //Box corner radius (smooth corners)
    float radius;

    //Used to make dialog transition animations
    float groupOpacity;

    //This texture will be rendered if render texture state is set to true
    sampler2D txtr;
};

//Rendering states
uniform States states;

//Properties of the button
uniform Properties properties;

//Fragment shader output
out vec4 fragColor;



float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

//Round corners
float applyRoundCorners(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
    //Radius of the corners
    float iRadius = properties.radius;
    vec2 halfRes = vec2(0.5*uniScale);

    //Compute box
    float b = udRoundBox( uv.xy*uniScale - halfRes, halfRes, iRadius );
   
    return 1. - b;
}

//Outline
float applyOutline(vec2 uv, float b) 
{
    b = applyRoundCorners(uv);
    
    b = 1. - b;

    //Fade of the outline (greater the value is smoother outline becomes)
    float fade = 1.225;

    //Outline
    float outV = (smoothstep(0.0, fade, 1. - b));
    outV *= (smoothstep(properties.outline.thickness + fade, properties.outline.thickness,1. - b));
   
    return outV;
}


void main(){

    //SET THE COLOR OF THE BOX
    fragColor = mix(properties.color, properties.color2, properties.colorMixVal);
    
    //TEXTURE RENDERING MODE
    if(states.renderTexture == 1){
        vec2 uv = TexCoords;
        
        fragColor = texture(properties.txtr, uv);
    }
    
    //CHARACTER RENDERING MODE
    else if(states.renderText == 1){
        //Render the text
        vec2 uv = TexCoords;
        uv.y = 1.0 - uv.y;
        fragColor.a = texture(properties.txtr, uv).r;
    }

    //ROUND CORNERS
    if(states.renderText == 0 && !(states.renderTexture == 1 && properties.radius <= 0.1)){

        float roundVal = applyRoundCorners(TexCoords); 
        
        if(roundVal < 0.2)
            fragColor.a = roundVal; //Give the outline or curves to the corners

        //OUTLINE
        if(properties.outline.state != 0 && states.renderTexture == 0){
            
            float outlineVal = applyOutline(TexCoords, roundVal); 
            
            //Only the outline
            if(properties.outline.state == 1){
                fragColor.a *= outlineVal;
            }
            
            //Additional outline
            else{
                vec3 outlineClr = mix(properties.outline.color, properties.outline.color2, properties.colorMixVal);
                fragColor.rgb = mix(fragColor.rgb,outlineClr,outlineVal);
            }
        }
    }
    

    //GROUP OPACITY CALCULATION
    fragColor.a *= properties.groupOpacity;
}