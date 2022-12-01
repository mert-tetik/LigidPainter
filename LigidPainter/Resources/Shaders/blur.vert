#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out vec2 blurTextureCoords[11];

uniform int isRenderVerticalBlur;

uniform float brushBlurVal;

uniform mat4 renderTextureProjection;

void main(){
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
    gl_Position =  renderTextureProjection * vec4(aPos, 1.0);
}