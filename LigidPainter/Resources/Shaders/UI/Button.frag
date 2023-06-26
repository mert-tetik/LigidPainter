//Most of the GUI elements is rendered here
//Such as buttons, range bars, checkbox etc.

#version 400 core

out vec4 fragColor;

in vec2 texCoords;
in vec2 uniScale;
//in vec3 fragPos;

uniform int outlineState;

uniform vec4 color;
uniform vec4 color2;
uniform vec3 outlineColor; //Only affected by the outlineExtra 
uniform vec3 outlineColor2;

uniform float colorMixVal;

uniform float thickness;
uniform float radius;

uniform float groupOpacity;

uniform int renderText;
uniform int renderTexture;
uniform sampler2D txtr;

float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv,bool doOutline) //! https://www.shadertoy.com/view/ldfSDj
{
    //Divider reduces the radius/size of the button 
    //So the outline won't be stuck out of the boundaries
    vec2 divider = vec2(1.0,1.0);
    //if(outline == 1)
    //    divider = vec2(1.0,1.3);

    //Radius of the corners
    float iRadius = radius;
    vec2 halfRes = vec2(0.5*uniScale);

    //Compute box
    float b = udRoundBox( uv.xy*uniScale * divider - halfRes * divider, halfRes, iRadius );
   
    //Fade of the outline (greater the value is smoother outline becomes)
    float fade = 1.115;

    //Outline
    float outV = (smoothstep(0.0, fade, 1. - b));
    outV *= (smoothstep(thickness + fade, thickness,1. - b));
   
    if(doOutline == true)
        return outV;
    else
        return 1. - b;
}


void main(){
    fragColor = mix(color,color2,colorMixVal);
    
    if(renderTexture == 1){
        //Render the texture
        vec2 uv = texCoords;
        fragColor = texture(txtr, uv);

        float roundVal = roundUp(texCoords,false);
        if(roundVal < 0.2)
            fragColor.a = roundVal;
    }
    else if(renderText == 1){
        //Render the text
        vec2 uv = texCoords;
        uv.y = 1.0 - uv.y;
        fragColor.a = texture(txtr, uv).r;
    }
    else if(radius != 0){
        //Create round corners
        //Returns only the outline if outline uniform is set to 1
        bool doOutline = false;
        if(outlineState == 1)
            doOutline = true;

        float roundVal = roundUp(texCoords,doOutline); 
        if(roundVal < 0.2)
            fragColor.a = roundVal; //Give the outline or curves to the corners
    }
    
    if(outlineState == 2){
        float outlineVal = roundUp(texCoords,true); 
        vec3 outlineClr = mix(outlineColor,outlineColor2,colorMixVal);
        fragColor.rgb = mix(fragColor.rgb,outlineClr,outlineVal);
    }

    fragColor.a *= groupOpacity;
}