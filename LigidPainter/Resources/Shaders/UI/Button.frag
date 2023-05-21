//Most of the GUI elements is rendered here
//Such as buttons, range bars, checkbox etc.

#version 330 core

out vec4 fragColor;

in vec2 texCoords;
//in vec3 fragPos;

uniform vec4 color;
uniform vec4 color2;
uniform float colorMixVal;

uniform int outline;
uniform float thickness;
uniform float radius;

uniform float width;
uniform float height;

uniform vec2 videoScale;

uniform int renderText;
uniform int renderTexture;
uniform sampler2D txtr;

float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
    //Divider reduces the radius/size of the button 
    //So the outline won't be stuck out of the boundaries
    vec2 divider = vec2(1.0,1.0);
    //if(outline == 1)
    //    divider = vec2(1.0,1.3);

    //Radius of the corners
    float iRadius = radius;
    vec2 halfRes = vec2(0.5*vec2(width,height));

    //Compute box
    float b = udRoundBox( uv.xy*vec2(width,height) * divider - halfRes * divider, halfRes, iRadius );
   
    //Fade of the outline (greater the value is smoother outline becomes)
    float fade = 1.115;

    //Outline
    float outV = (smoothstep(0.0, fade, 1. - b));
    outV *= (smoothstep(thickness + fade, thickness,1. - b));
   
    if(outline == 1)
        return outV;
    else
        return 1. - b;
}

void main(){
    fragColor = mix(color,color2,colorMixVal);
    
    if(renderTexture == 1){
        //Render the texture
        vec2 uv = texCoords;
        fragColor = texture(txtr, uv).rgba;
        fragColor.a = 0.5; 
    }
    else if(renderText == 1){
        //Render the text
        vec2 uv = texCoords;
        uv.y = 1.0 - uv.y;
        fragColor.a = texture(txtr, uv).r;
    }
    else if(radius != 0)
    //Create round corners
    //Returns only the outline if outline uniform is set to 1
        fragColor.a = roundUp(texCoords); 
}