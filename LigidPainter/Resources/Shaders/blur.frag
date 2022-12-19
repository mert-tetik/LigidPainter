#version 330 core

out vec4 color;

uniform sampler2D inputTexture;

in vec2 blurTextureCoords[53];

void main(){
    //0.0003,	0.0004,	0.0007,	0.0012,	0.0019,	0.0029,	0.0044,	0.0064,	0.0090,	0.0124,	0.0166,	0.0216,	0.0274,	0.0337,	0.0404,	0.0470,	0.0532,	0.0587,	0.0629,	0.0655,	0.0665,	0.0655,	0.0629,	0.0587,	0.0532,	0.0470,	0.0404,	0.0337,	0.0274,	0.0216,	0.0166,	0.0124,	0.0090,	0.0064,	0.0044,	0.0029,	0.0019,	0.0012,	0.0007,	0.0004,	0.0003
    
    

    color = vec4(0.0);
    color += texture(inputTexture, blurTextureCoords[0]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[1]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[2]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[3]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[4]) * 0.0001;
    color += texture(inputTexture, blurTextureCoords[5]) * 0.0001;
    color += texture(inputTexture, blurTextureCoords[6]) * 0.0003;
    color += texture(inputTexture, blurTextureCoords[7]) * 0.0004;
    color += texture(inputTexture, blurTextureCoords[8]) * 0.0007;
    color += texture(inputTexture, blurTextureCoords[9]) * 0.0012;
    color += texture(inputTexture, blurTextureCoords[10]) * 0.0019;
    color += texture(inputTexture, blurTextureCoords[11]) * 0.0029;
    color += texture(inputTexture, blurTextureCoords[12]) * 0.0044;
    color += texture(inputTexture, blurTextureCoords[13]) * 0.0064;
    color += texture(inputTexture, blurTextureCoords[14]) * 0.0090;
    color += texture(inputTexture, blurTextureCoords[15]) * 0.0124;
    color += texture(inputTexture, blurTextureCoords[16]) * 0.0166;
    color += texture(inputTexture, blurTextureCoords[17]) * 0.0216;
    color += texture(inputTexture, blurTextureCoords[18]) * 0.0274;
    color += texture(inputTexture, blurTextureCoords[19]) * 0.0337;
    color += texture(inputTexture, blurTextureCoords[20]) * 0.0403;
    color += texture(inputTexture, blurTextureCoords[21]) * 0.0470;
    color += texture(inputTexture, blurTextureCoords[22]) * 0.0532;
    color += texture(inputTexture, blurTextureCoords[23]) * 0.0586;
    color += texture(inputTexture, blurTextureCoords[24]) * 0.0628;
    color += texture(inputTexture, blurTextureCoords[25]) * 0.0655;
    color += texture(inputTexture, blurTextureCoords[26]) * 0.0664;
    color += texture(inputTexture, blurTextureCoords[27]) * 0.0655;
    color += texture(inputTexture, blurTextureCoords[28]) * 0.0628;
    color += texture(inputTexture, blurTextureCoords[29]) * 0.0586;
    color += texture(inputTexture, blurTextureCoords[30]) * 0.0532;
    color += texture(inputTexture, blurTextureCoords[31]) * 0.0470;
    color += texture(inputTexture, blurTextureCoords[32]) * 0.0403;
    color += texture(inputTexture, blurTextureCoords[33]) * 0.0337;
    color += texture(inputTexture, blurTextureCoords[34]) * 0.0274;
    color += texture(inputTexture, blurTextureCoords[35]) * 0.0216;
    color += texture(inputTexture, blurTextureCoords[36]) * 0.0166;
    color += texture(inputTexture, blurTextureCoords[37]) * 0.0124;
    color += texture(inputTexture, blurTextureCoords[38]) * 0.0090;
    color += texture(inputTexture, blurTextureCoords[39]) * 0.0064;
    color += texture(inputTexture, blurTextureCoords[40]) * 0.0044;
    color += texture(inputTexture, blurTextureCoords[41]) * 0.0029;
    color += texture(inputTexture, blurTextureCoords[42]) * 0.0019;
    color += texture(inputTexture, blurTextureCoords[43]) * 0.0012;
    color += texture(inputTexture, blurTextureCoords[44]) * 0.0007;
    color += texture(inputTexture, blurTextureCoords[45]) * 0.0004;
    color += texture(inputTexture, blurTextureCoords[46]) * 0.0003;
    color += texture(inputTexture, blurTextureCoords[47]) * 0.0001;
    color += texture(inputTexture, blurTextureCoords[48]) * 0.0001;
    color += texture(inputTexture, blurTextureCoords[49]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[50]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[51]) * 0.0000;
    color += texture(inputTexture, blurTextureCoords[52]) * 0.0000;
    	
    	

}