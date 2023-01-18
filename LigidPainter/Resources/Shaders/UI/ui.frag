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
uniform int drawTxtrDemonstrator;
uniform sampler2D currentTexture;

out vec4 color;

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
                  //Brush indicator here
                  color = vec4(uiColor.rgb, max(texture(modifiedMaskTexture, TexCoords)/3.0,0.0)); 
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
         vec4 sampled = vec4(uiColor.rgb, texture(text, TexCoords).r);
         color = vec4(textColor,1) * sampled;
      }
   }

}