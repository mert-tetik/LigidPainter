#version 330 core

in vec2 TexCoords;

uniform int isTextF;
uniform sampler2D text;
uniform sampler2D modifiedMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;
uniform vec3 uiColor;
uniform vec3 uiTransitionColor;
uniform float uiTransitionMixVal;
uniform float uiOpacity;
uniform int drawBrushIndicator;

out vec4 color;

void main() {

   if(isTextF == 0) {
         if(isUiTextureUsed == 0) 
         {
            if(drawBrushIndicator == 1)
            {
               //Brush indicator here
               color = vec4(uiColor, max(texture(modifiedMaskTexture, TexCoords)/2,0.0)); 
            }
            else
            {
               //Ui here
               color = vec4(mix(uiColor,uiTransitionColor,uiTransitionMixVal), uiOpacity);
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
      vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
      color = vec4(textColor,1) * sampled;
   }
}