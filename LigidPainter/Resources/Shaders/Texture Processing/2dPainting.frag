#version 330 core

out vec4 fragColor;

uniform sampler2D modifiedMaskTexture;

uniform int invert;

in vec2 texCoords;
in vec3 color;

void main(){
    float intensity;
    if(invert == 0)
        intensity = texture(modifiedMaskTexture,texCoords).r;
    else
        intensity = 1.0 - texture(modifiedMaskTexture,texCoords).r;

    fragColor = vec4(vec3(intensity * color),intensity);
}