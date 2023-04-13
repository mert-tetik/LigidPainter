#version 330 core

out vec4 fragColor;

uniform sampler2D modifiedMaskTexture;
uniform sampler2D backTxtr;
uniform float mousex;
uniform float mousey;

uniform int invert;

in vec2 texCoords;
in vec3 color;

void main(){
    float intensity;
    if(invert == 0)
        fragColor = vec4(vec3(texture(modifiedMaskTexture,texCoords).rgb),texture(modifiedMaskTexture,texCoords).a);
    else
        fragColor = vec4(vec3(1.0 - texture(modifiedMaskTexture,texCoords).r),1);

}