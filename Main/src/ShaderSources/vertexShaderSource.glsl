#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
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

uniform mat4 renderTextureProjection;
uniform int isRenderTextureModeV;


void main() {
   Pos = aPos;
   TexCoords = aTexCoords;
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = aNormal;
   if(isRenderTextureModeV == 0) {
      if(isText == 0) {
         if(isTwoDimensional == 0) {
            gl_Position = projection * view * vec4(aPos, 0.5);
            projectedPos = projection * view * vec4(aPos, 0.5); //Caution
         } else {
            gl_Position = TextProjection * vec4(aPos, 1.0);
         }
      } else if(isText == 1) {
         gl_Position = TextProjection * vec4(aPos, 1.0);
         TextTexCoords = aTexCoords;
      }
   } else {
      gl_Position = renderTextureProjection * vec4(aTexCoords, 0, 1);

      projectedPos = projection * view * vec4(aPos, 0.5); //Caution
   }
}