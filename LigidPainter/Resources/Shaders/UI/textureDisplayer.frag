#version 330 core

in vec2 TexCoords;

uniform sampler2D currentTexture;

layout(location = 0) out vec4 color;


float mu(vec2 a){return a.x*a.y;}
float CheckerSignMuFract(vec2 u)
{
    return sign(mu(.5-fract(u)));
}

void main() {
    float checker = CheckerSignMuFract(TexCoords*15.f);
    checker+=1.5;

    color = vec4(mix(vec3(checker),texture(currentTexture, TexCoords).rgb,texture(currentTexture, TexCoords).a) ,1.0); 
}