#version 330 core

out vec4 color;

uniform sampler2D inputTexture;

in vec2 blurTextureCoords[11];

void main(){
    color = vec4(0.0);
    color += texture(inputTexture, blurTextureCoords[0]) * 0.0093;
    color += texture(inputTexture, blurTextureCoords[1]) * 0.028002;
    color += texture(inputTexture, blurTextureCoords[2]) * 0.065984;
    color += texture(inputTexture, blurTextureCoords[3]) * 0.121703;
    color += texture(inputTexture, blurTextureCoords[4]) * 0.175713;
    color += texture(inputTexture, blurTextureCoords[5]) * 0.198596;
    color += texture(inputTexture, blurTextureCoords[6]) * 0.175713;
    color += texture(inputTexture, blurTextureCoords[7]) * 0.121703;
    color += texture(inputTexture, blurTextureCoords[8]) * 0.065984;
    color += texture(inputTexture, blurTextureCoords[9]) * 0.028002;
    color += texture(inputTexture, blurTextureCoords[10]) * 0.0093;
}