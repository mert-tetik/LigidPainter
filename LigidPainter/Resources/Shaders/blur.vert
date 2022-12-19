#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out vec2 blurTextureCoords[53];

uniform int isRenderVerticalBlur;

uniform float brushBlurVal;

uniform mat4 renderTextureProjection;


void main(){
    vec2 centerTexCoords = aTexCoords;
    float pixelSize = 1.0 / brushBlurVal;

    for(int i=-26;i<=26;i++){
        if(isRenderVerticalBlur == 0){
           blurTextureCoords[i+26] = centerTexCoords + vec2(pixelSize*(i)/2.0,0); 
        }
        else{
           blurTextureCoords[i+26] = centerTexCoords + vec2(0,pixelSize*(i)/2.0); 
        }
    }
    gl_Position =  renderTextureProjection * vec4(aPos, 1.0);
}