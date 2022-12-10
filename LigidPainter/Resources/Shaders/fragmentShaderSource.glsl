#version 330 core

struct Material {
   sampler2D diffuse;
};
uniform Material material;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

in vec4 projectedPos;
in vec4 mirroredProjectedPos;

uniform sampler2D screenMaskTexture;
uniform sampler2D mirroredScreenMaskTexture;
uniform int useMirror;
uniform int isRenderScreenMaskMode;
uniform int verticalMirror;
uniform vec3 drawColor;


uniform sampler2D depthTexture; //Screen rendered with depth color
uniform sampler2D mirroredDepthTexture; //Screen rendered with depth color

uniform int isRenderTextureMode;
uniform int renderDepth;
uniform int renderMaskBrush;


uniform int isTextF;
uniform sampler2D text;
uniform sampler2D modifiedMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;

uniform int is2D;
uniform vec3 viewPos;
uniform vec3 mirroredViewPos;
uniform int isAxisPointer;

uniform vec3 uiColor;
uniform vec3 uiTransitionColor;
uniform float uiTransitionMixVal;

uniform float uiOpacity;

uniform int isColorBox;

uniform int drawBrushIndicator;

out vec4 color;

uniform int whiteRendering;

uniform sampler2D uvMask;
uniform int interpretWithUvMask;


float far = 10.0f;
float near = 0.1f;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

bool isPainted(vec3 uv, bool isMirrored) { //Use mirrored depth texture if isMirrored is true
   float drawZ;
   if(!isMirrored){
      drawZ = texture2D(depthTexture, uv.xy).b;
   }
   else{
      drawZ = texture2D(mirroredDepthTexture, uv.xy).b; 
   }


   vec3 direction;
   if(isMirrored){
      direction = mirroredViewPos - Pos;
   }
   else{
      direction = viewPos - Pos;
   }
   float dotProd = dot(normalize(direction),normalize(Normal));

   if(dotProd < 0.2){
      return false;
   }  
   else{
      return abs(drawZ - linearizeDepth(uv.z)/far) < 0.005;
   }
}


vec3 getPaintedDiffuse(){
      //Painting
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);
   vec3 mirroredScreenPos = mirroredProjectedPos.xyz / mirroredProjectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

   float intensity = 0.0f;
   float mirroredIntensity = 0.0f;
   if(isPainted(screenPos,false)) 
   {
      intensity = texture2D(screenMaskTexture, screenPos.xy).r;
   }
   if(isPainted(mirroredScreenPos, true)) 
   {
      mirroredIntensity = texture2D((mirroredScreenMaskTexture), mirroredScreenPos.xy).r;
   }
   
    // ambient
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));
   vec3 diffuseDrawMix = mix(diffuseClr, drawColor, intensity);
   vec3 mirroredDiffuseDrawMix;
   if(useMirror == 1){
      mirroredDiffuseDrawMix = mix(diffuseDrawMix, drawColor, mirroredIntensity);
   }
   else{
      mirroredDiffuseDrawMix = diffuseDrawMix;
   }
   
   return mirroredDiffuseDrawMix;
}


void main() {

      if(isRenderTextureMode == 0) {
      if(isColorBox == 0) {
         if(isTextF == 0) {
            if(is2D == 0) {
                if(isAxisPointer == 1) 
                {
                   //Axis pointer here
                   color = vec4((abs(Pos.x) / 20), (abs(Pos.y) / 20), (abs(Pos.z) / 20), 1);
                }
            } 
            else 
            { //is2D == true
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
         }
         else 
         {
            //Text here
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor,1) * sampled;
         }
      } 
   } 
   else { //Render texture
      if(isRenderScreenMaskMode == 0){
         if(renderMaskBrush == 1)
         {
            //Mask texture here
            color = texture(modifiedMaskTexture, TexCoords);
         }
         else
         {
               if(renderDepth == 0)
               {
                  //Diffuse result here
                  if(interpretWithUvMask == 0){
                     if(whiteRendering == 0){
                        vec3 paintedDiffuse = getPaintedDiffuse();
                        color = vec4(paintedDiffuse, 1);
                     }
                     else{
                        color = vec4(1);
                     }
                  }
                  else{
                     //Paint around
                     if(texture2D(uvMask,vec2(TexCoords.x - 0.004,TexCoords.y)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x - 0.004,TexCoords.y)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x + 0.004,TexCoords.y)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x + 0.004,TexCoords.y)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x,TexCoords.y - 0.004)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x ,TexCoords.y - 0.004)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x,TexCoords.y + 0.004)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x ,TexCoords.y + 0.004)));
                     }
                     else{
                        color = vec4(texture2D(material.diffuse,TexCoords));
                     }
                  }
                  
               }
         }
      }
      else{
         //Mirrored mask texture here
         if(verticalMirror == 0){
            color = texture(screenMaskTexture, vec2(1.0 - TexCoords.x,TexCoords.y));
         }
         else{
            color = texture(screenMaskTexture, vec2(TexCoords.x , 1.0 - TexCoords.y));
         }
      }
   }
}