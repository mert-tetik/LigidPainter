#version 330 core

in vec2 TexCoords;

uniform int isTextF;
uniform sampler2D text;
uniform sampler2D modifiedMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;

uniform vec4 uiColor;

uniform vec4 uiTransitionColor;
uniform float uiTransitionMixVal;

layout(location = 0) out vec4 color;

const float PI = 3.14159265;

uniform float width;
uniform float height;
uniform float radius;
uniform int outline;

float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
   float divider = 1.05;
    //TODO : Check if round corners effect texture rendering+
    // setup
    float iRadius = radius;
    vec2 halfRes = vec2(0.5*vec2(1920*width,1080*height));

    // compute box
    float b = udRoundBox( uv.xy*vec2(1920*width,1080*height)*divider - halfRes*divider, halfRes, iRadius );
    
    // colorize (red / black )
	vec3 c = mix( vec3(1.0,0.0,0.0), vec3(0.0,0.0,0.0), smoothstep(0.0,1.0,b) );
   
   vec3 circleColor = vec3(1.0, 1.0, 1.0);
   float thickness = 2.5;
   float fade = 0.005;
   
   vec3 color = vec3(smoothstep(0.0, fade, b));
   color *= vec3(smoothstep(thickness + fade, thickness, b)) * circleColor;
   
    return color.r;
}

void main() {
      if(isTextF == 0) {
            if(isUiTextureUsed == 0) 
            {
               //Ui here
               color = vec4(mix(uiColor,uiTransitionColor,uiTransitionMixVal));
               if(radius < 100)
                  color.a *= roundUp(TexCoords);
            } 
            else 
            {
               //Mask texture box here
               //TODO Seperate
               color = vec4(vec3(texture(modifiedMaskTexture, TexCoords).rgb),1);
               if(roundUp(TexCoords) < 0.05)
                  color.a = 0.;
            }
         }
      else 
      {
         //Text here
         vec2 uv = TexCoords;
         uv.y = 1. - uv.y;
         vec4 cColor = mix(uiColor,uiTransitionColor,uiTransitionMixVal); 
         vec4 sampled = vec4(cColor.rgb, texture(text, uv).r);
         color = vec4(textColor,1) * sampled * uiColor.a;
      }
}