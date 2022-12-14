#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 mirroredView;

uniform mat4 projection;
uniform int isTwoDimensional;

uniform mat4 renderTextureProjection;
uniform mat4 renderTrans;

uniform int use3d;


out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;

out vec4 projectedPos;
out vec4 mirroredProjectedPos;


void main() {
   
   Pos = aPos;
   TexCoords = aTexCoords;
   Normal = aNormal;
   projectedPos = projection * view * vec4(aPos, 0.5); 
   mirroredProjectedPos = projection * mirroredView * vec4(aPos, 0.5); 

   if(use3d == 0){
      if(isTwoDimensional == 0){
         gl_Position = renderTextureProjection * vec4(aTexCoords, 0, 1);
      }
      else{
         gl_Position = renderTrans * renderTextureProjection * vec4(aPos, 1.0);
      }
   }
   else{
      gl_Position = projectedPos;
   }
}