#version 400 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D maskTexture;
uniform sampler2D texture_black;
uniform sampler2D texture_white;
uniform int maskAlpha;
uniform int normalMapMode;
uniform int invert;
uniform float offset; // 0 - 1 (0.5 = normal)

void main(){
    float mask = texture(maskTexture, TexCoords).r;
    
    if(offset <= 0.5)
        mask = mix(0., mask, offset*2.);
    else 
        mask = mix(1., mask, 1. - (offset*2. - 1.));


    vec4 param1;
    vec4 param2;
    float param3;

    if(maskAlpha == 1){
        param1 = texture(texture_black, TexCoords);
        param2 = texture(texture_white, TexCoords);
        param3 = texture(maskTexture, TexCoords).a;
    }
    else if(normalMapMode == 1){
        vec4 normalMapVal = texture(maskTexture, TexCoords);
        normalMapVal.rgb -= 0.5;
        mask = max(normalMapVal.r, normalMapVal.g);
        
        param1 = texture(texture_black, TexCoords);
        param2 = texture(texture_white, TexCoords);
        param3 = mask;
    }
    else{
        param1 = texture(texture_black, TexCoords);
        param2 = texture(texture_white, TexCoords);
        param3 = mask;
    }

    if(invert == 0)
        fragColor = mix(param1, param2, param3);
    else
        fragColor = mix(param2, param1, param3);
}