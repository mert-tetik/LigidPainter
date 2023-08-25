#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;
uniform vec2 txtrResolution;
uniform vec2 destTxtrResolution;
uniform int wrapingIndex;
uniform vec3 wrapingColor;
uniform vec2 originPoint;

out vec4 color;

void main(){
    vec2 uv;
    if(wrapingIndex != 1 && wrapingIndex != 2)
        uv = (TexCoords - originPoint) / (txtrResolution / destTxtrResolution);
    else
        uv = (TexCoords) / (txtrResolution / destTxtrResolution);
    
    /*
    if(uv.x < originPoint.x - (uv / (txtrResolution.x/destTxtrResolution.x)) || uv.y < originPoint.y - (uv / (txtrResolution.y/destTxtrResolution.y)) || uv.x > originPoint.x + (uv / (txtrResolution.x/destTxtrResolution.x)) || uv.y > originPoint.y + (uv / (txtrResolution.y/destTxtrResolution.y)))
        color.a = 0.;
    else
        color.a = texture(txtr, uv).a;
      */  
    color.rgb = texture(txtr, uv).rgb;
}