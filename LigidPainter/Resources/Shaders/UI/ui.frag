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

void main() {
      if(isTextF == 0) {
            if(isUiTextureUsed == 0) 
            {
               //Ui here
               color = vec4(mix(uiColor,uiTransitionColor,uiTransitionMixVal));
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