#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Color_Convertions.frag)
vec3 rgb2hsv( vec3 rgb);

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D IDTexture;
uniform vec3 wbr;
uniform vec3 pbc;
uniform vec3 gyo;


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

    maskWhite *= wbr[0];
    maskBlack *= wbr[1];
    maskRed *= wbr[2];
    maskPink *= pbc[0];
    maskBlue *= pbc[1];
    maskCyan *= pbc[2];
    maskGreen *= gyo[0];
    maskYellow *= gyo[1];
    maskOrange *= gyo[2];

    vec3 res = vec3(0);
    res = mix(res, vec3(1.), maskBlack);
    res = mix(res, vec3(1.), maskWhite);
    res = mix(res, vec3(1.), maskRed);
    res = mix(res, vec3(1.), maskGreen);
    res = mix(res, vec3(1.), maskBlue);
    res = mix(res, vec3(1.), maskYellow);
    res = mix(res, vec3(1.), maskOrange);
    res = mix(res, vec3(1.), maskCyan);
    res = mix(res, vec3(1.), maskPink);

    fragColor = vec4(res, 1.);
}