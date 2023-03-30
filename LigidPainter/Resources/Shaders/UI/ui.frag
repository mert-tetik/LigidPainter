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

void main() {
   if(drawTxtrDemonstrator == 1){
      color = vec4(texture(currentTexture, TexCoords).rgb ,1.0); 
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