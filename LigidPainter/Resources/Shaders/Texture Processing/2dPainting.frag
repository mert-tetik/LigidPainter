#version 330 core

out vec4 fragColor;

uniform sampler2D modifiedMaskTexture;

uniform int invert;

in vec2 texCoords;
in vec3 color;

void main(){
    float intensity;
    if(invert == 0)
        intensity = texture(modifiedMaskTexture,texCoords).a;
    else
        intensity = 1.0 - texture(modifiedMaskTexture,texCoords).a;

    fragColor = vec4(vec3(intensity * texture(modifiedMaskTexture,texCoords).rgb),intensity);
}