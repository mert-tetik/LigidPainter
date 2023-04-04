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


uniform int drawBrushIndicator;
uniform int drawCircle;
uniform int drawTxtrDemonstrator;
uniform sampler2D currentTexture;

layout(location = 0) out vec4 color;


#define patternCount 23.0

#define PI 3.141592

#define pi acos(-1.)
#define dd(a) dot(a,a)
#define sat(a) clamp(a,0.,1.)
#define aA(z) (z/min(iResolution.x,iResolution.y))


float mi(vec2 a){return min(a.x,a.y);}
float mi(vec3 a){return min(a.x,mi(a.yz));}
float ma(vec2 a){return max(a.x,a.y);}
float mu(vec2 a){return a.x*a.y;}
float ad(vec2 a){return a.x+a.y;}
float su(vec2 a){return a.x-a.y;}
float sq2(float a){return a*a;}
float CheckerSignMuFract(vec2 u)
{
    return sign(mu(.5-fract(u)));
}

void main() {
   if(drawTxtrDemonstrator == 1){
      float checker = CheckerSignMuFract(TexCoords*15.f);
      checker+=1.5;

      color = vec4(mix(vec3(checker),texture(currentTexture, TexCoords).rgb,texture(currentTexture, TexCoords).a) ,1.0); 
   }
   else{
      if(isTextF == 0) {
            if(isUiTextureUsed == 0) 
            {
               if(drawBrushIndicator == 1)
               {
                  if(drawCircle == 0){
                     //Brush indicator here
                     float maskTxtr = max(texture(modifiedMaskTexture, TexCoords),0.0).r;

                     const float thickness = 0.02;

                     float maskTxtrLeft = max(texture(modifiedMaskTexture, TexCoords - vec2(thickness,0)),0.0).r;
                     float maskTxtrRight = max(texture(modifiedMaskTexture, TexCoords + vec2(thickness,0)),0.0).r;
                     float maskTxtrTop = max(texture(modifiedMaskTexture, TexCoords - vec2(0,thickness)),0.0).r;
                     float maskTxtrBot = max(texture(modifiedMaskTexture, TexCoords + vec2(0,thickness)),0.0).r;

                     float maskResult = 0;

                     if(maskTxtr > 0.02){
                        if(maskTxtrLeft < 0.02 || maskTxtrRight < 0.02 || maskTxtrTop < 0.02 || maskTxtrBot < 0.02){
                           maskResult = 1.0;
                        }
                     }
                     color = vec4(uiColor.rgb, maskResult);
                  }
                  else{
                     //!I stoled it from https://www.shadertoy.com/view/ssdSD2
                     // Parameters
                     float thickness = 0.04;
                     // Control with mouse
                     // thickness = iMouse.x / iResolution.x;
                     float fade = 0.005;

                     // -1 -> 1 local space, adjusted for aspect ratio
                     vec2 uv = TexCoords * 2.0 - 1.0;

                     // Calculate distance and fill circle with white
                     float distance = 1.0 - length(uv);
                     vec3 clr = vec3(smoothstep(0.0, fade, distance));
                     clr *= vec3(smoothstep(thickness + fade, thickness, distance));

                     // Set output color
                     color = vec4(clr, clr.r);
                  }
               }
               else
               {
                  //Ui here
                  color = vec4(mix(uiColor,uiTransitionColor,uiTransitionMixVal));
               }
            } 
            else 
            {
               //Mask texture box here
               color = texture(modifiedMaskTexture, TexCoords);
            }
         }
      else 
      {
         //Text here
         vec2 uv = TexCoords;
         uv.y = 1. - uv.y;
         vec4 sampled = vec4(uiColor.rgb, texture(text, uv).r);
         color = vec4(textColor,1) * sampled * uiColor.a;
      }
   }

}