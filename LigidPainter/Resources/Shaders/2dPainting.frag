#version 330 core

out vec4 fragColor;

uniform sampler2D modifiedMaskTexture;

in vec2 texCoords;
in vec3 color;

void main(){
    
    float intensity = texture2D(modifiedMaskTexture,texCoords).r;

    fragColor = vec4(intensity);
}