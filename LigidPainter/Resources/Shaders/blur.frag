#version 330 core

out vec4 color;

uniform sampler2D inputTexture;

in vec2 blurTextureCoords[41];

void main(){
    //0.0000,	0.0002,	0.0060,	0.0606,	0.2417,	0.3829,	0.2417,	0.0606,	0.0060,	0.0002,	0.0000

    color = vec4(0.0);
    color += texture(inputTexture, blurTextureCoords[0]) * 0.0210;
    color += texture(inputTexture, blurTextureCoords[1]) * 0.0214;
    color += texture(inputTexture, blurTextureCoords[2]) * 0.0219;
    color += texture(inputTexture, blurTextureCoords[3]) * 0.0223;
    color += texture(inputTexture, blurTextureCoords[4]) * 0.0228;
    color += texture(inputTexture, blurTextureCoords[5]) * 0.0232;
    color += texture(inputTexture, blurTextureCoords[6]) * 0.0236;
    color += texture(inputTexture, blurTextureCoords[7]) * 0.0239;
    color += texture(inputTexture, blurTextureCoords[8]) * 0.0243;
    color += texture(inputTexture, blurTextureCoords[9]) * 0.0246;
    color += texture(inputTexture, blurTextureCoords[10]) * 0.0249;
    color += texture(inputTexture, blurTextureCoords[11]) * 0.0252;
    color += texture(inputTexture, blurTextureCoords[12]) * 0.0254;
    color += texture(inputTexture, blurTextureCoords[13]) * 0.0256;
    color += texture(inputTexture, blurTextureCoords[14]) * 0.0258;
    color += texture(inputTexture, blurTextureCoords[15]) * 0.0260;
    color += texture(inputTexture, blurTextureCoords[16]) * 0.0261;
    color += texture(inputTexture, blurTextureCoords[17]) * 0.0262;
    color += texture(inputTexture, blurTextureCoords[18]) * 0.0263;
    color += texture(inputTexture, blurTextureCoords[19]) * 0.0264;
    color += texture(inputTexture, blurTextureCoords[20]) * 0.0264;
    color += texture(inputTexture, blurTextureCoords[21]) * 0.0264;
    color += texture(inputTexture, blurTextureCoords[22]) * 0.0263;
    color += texture(inputTexture, blurTextureCoords[23]) * 0.0262;
    color += texture(inputTexture, blurTextureCoords[24]) * 0.0261;
    color += texture(inputTexture, blurTextureCoords[25]) * 0.0260;
    color += texture(inputTexture, blurTextureCoords[26]) * 0.0258;
    color += texture(inputTexture, blurTextureCoords[27]) * 0.0256;
    color += texture(inputTexture, blurTextureCoords[28]) * 0.0254;
    color += texture(inputTexture, blurTextureCoords[29]) * 0.0252;
    color += texture(inputTexture, blurTextureCoords[30]) * 0.0249;
    color += texture(inputTexture, blurTextureCoords[31]) * 0.0246;
    color += texture(inputTexture, blurTextureCoords[32]) * 0.0243;
    color += texture(inputTexture, blurTextureCoords[33]) * 0.0239;
    color += texture(inputTexture, blurTextureCoords[34]) * 0.0236;
    color += texture(inputTexture, blurTextureCoords[35]) * 0.0232;
    color += texture(inputTexture, blurTextureCoords[36]) * 0.0228;
    color += texture(inputTexture, blurTextureCoords[37]) * 0.0223;
    color += texture(inputTexture, blurTextureCoords[38]) * 0.0219;
    color += texture(inputTexture, blurTextureCoords[39]) * 0.0214;
    color += texture(inputTexture, blurTextureCoords[40]) * 0.0210;

}