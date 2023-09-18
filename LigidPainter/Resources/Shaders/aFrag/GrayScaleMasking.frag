#version 400 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D maskTexture;
uniform sampler2D texture_black;
uniform sampler2D texture_white;
uniform float offset; // 0 - 1 (0.5 = normal)

void main(){
    float mask = texture(maskTexture, TexCoords).r;
    
    if(offset <= 0.5)
        mask = mix(0., mask, offset*2.);
    else 
        mask = mix(1., mask, 1. - (offset*2. - 1.));

    fragColor = vec4(mix(texture(texture_black, TexCoords), texture(texture_white, TexCoords), mask * texture(texture_white, TexCoords).a));
}