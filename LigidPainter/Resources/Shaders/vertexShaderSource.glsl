#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 mirroredView;

uniform mat4 projection;
uniform int isTwoDimensional;

uniform mat4 TextProjection;
uniform int isText;

out vec2 TextTexCoords;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec3 Pos;

out vec4 projectedPos;
out vec4 mirroredProjectedPos;

uniform mat4 renderTextureProjection;
uniform int isRenderTextureModeV;

uniform int isRenderVerticalBlur;

out vec2 blurTextureCoords[11];
uniform float brushBlurVal;

uniform int renderMirroredDepthV;

uniform mat4 renderTrans;

void main() {
   
   Pos = aPos;
   TexCoords = aTexCoords;
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = aNormal;
   projectedPos = projection * view * vec4(aPos, 0.5); 
   mirroredProjectedPos = projection * mirroredView * vec4(aPos, 0.5); 

   vec2 centerTexCoords = aTexCoords;
   float pixelSize = 1.0 / brushBlurVal;

   for(int i=-5;i<=5;i++){
      if(isRenderVerticalBlur == 0){
         blurTextureCoords[i+5] = centerTexCoords + vec2(pixelSize*i,0); 
      }
      else{
         blurTextureCoords[i+5] = centerTexCoords + vec2(0,pixelSize*i); 
      }
   }

   if(isRenderTextureModeV == 0) {
      if(isText == 0) {
         if(isTwoDimensional == 0) {
            if(renderMirroredDepthV == 0){
               gl_Position = projection * view * vec4(aPos, 0.5);
            }
            else{
               gl_Position = projection * mirroredView * vec4(aPos, 0.5);
            }
         } else {
            gl_Position = TextProjection * vec4(aPos, 1.0);
         }
      } else if(isText == 1) {
         gl_Position = TextProjection * vec4(aPos, 1.0);
         TextTexCoords = aTexCoords;
      }
   } else {
      if(isTwoDimensional == 0){
         gl_Position = renderTextureProjection * vec4(aTexCoords, 0, 1);
      }
      else{
         gl_Position = renderTrans * renderTextureProjection * vec4(aPos, 1.0);
      }
   }
}