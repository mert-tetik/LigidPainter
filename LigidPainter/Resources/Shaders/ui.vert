#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 view;

uniform mat4 projection;
uniform int isTwoDimensional;

uniform mat4 TextProjection;
uniform int isText;

out vec2 TexCoords;

uniform mat4 renderTextureProjection;
uniform int isRenderTextureModeV;

uniform int renderMirroredDepthV;

uniform mat4 renderTrans;


void main() {
   
   TexCoords = aTexCoords;

   gl_Position = TextProjection * vec4(aPos, 1.0);
       
}