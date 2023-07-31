#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Color_Convertions.frag)
vec3 rgb2hsv( vec3 rgb);

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D IDTexture;
uniform sampler2D texture_black;
uniform sampler2D texture_white;
uniform sampler2D texture_red;
uniform sampler2D texture_green;
uniform sampler2D texture_blue;
uniform sampler2D texture_yellow;
uniform sampler2D texture_orange;
uniform sampler2D texture_cyan;
uniform sampler2D texture_pink;


void main(){
    vec3 id = texture(IDTexture, TexCoords).rgb;

    vec3 idHSV = rgb2hsv(id);
    idHSV.r *= 360.;
    idHSV.g *= 100.;
    idHSV.b *= 100.;

    float maskWhite = 0;
    float maskBlack = 0;
    float maskRed = 0;
    float maskPink = 0;
    float maskBlue = 0;
    float maskCyan = 0;
    float maskGreen = 0;
    float maskYellow = 0;
    float maskOrange = 0;

    if(idHSV.g < 5 && idHSV.b > 90)
        maskWhite = idHSV.b / 100.;
    else if(idHSV.g < 5 && idHSV.b < 10)
        maskBlack = idHSV.b / 100.;
    else{
        if(idHSV.r < 15 || idHSV.r > 345)
            maskRed = idHSV.b / 100.;
        else if(idHSV.r > 265 && idHSV.r <= 345)
            maskPink = idHSV.b / 100.;
        else if(idHSV.r > 205 && idHSV.r <= 265)
            maskBlue = idHSV.b / 100.;
        else if(idHSV.r > 175 && idHSV.r <= 205)
            maskCyan = idHSV.b / 100.;
        else if(idHSV.r > 70 && idHSV.r <= 175)
            maskGreen = idHSV.b / 100.;
        else if(idHSV.r > 55 && idHSV.r <= 70) 
            maskYellow = idHSV.b / 100.;
        else 
            maskOrange = idHSV.b / 100.;
    } 

    vec3 blackVal = texture(texture_black, TexCoords).rgb; 
    vec3 whiteVal = texture(texture_white, TexCoords).rgb; 
    vec3 redVal = texture(texture_red, TexCoords).rgb; 
    vec3 greenVal = texture(texture_green, TexCoords).rgb; 
    vec3 blueVal = texture(texture_blue, TexCoords).rgb; 
    vec3 yellowVal = texture(texture_yellow, TexCoords).rgb; 
    vec3 orangeVal = texture(texture_orange, TexCoords).rgb; 
    vec3 cyanVal = texture(texture_cyan, TexCoords).rgb; 
    vec3 pinkVal = texture(texture_pink, TexCoords).rgb; 

    vec3 res = vec3(0);
    res = mix(res, blackVal, maskBlack);
    res = mix(res, whiteVal, maskWhite);
    res = mix(res, redVal, maskRed);
    res = mix(res, greenVal, maskGreen);
    res = mix(res, blueVal, maskBlue);
    res = mix(res, yellowVal, maskYellow);
    res = mix(res, orangeVal, maskOrange);
    res = mix(res, cyanVal, maskCyan);
    res = mix(res, pinkVal, maskPink);

    fragColor = vec4(res, 1.);
}