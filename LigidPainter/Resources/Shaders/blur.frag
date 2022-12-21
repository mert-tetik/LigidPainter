#version 330 core

out vec4 color;

uniform sampler2D inputTexture;


uniform int kernel_sizeF;

uniform float oneDimensionalGaussF1[61];
uniform float oneDimensionalGaussF2[61];

uniform int isRenderVerticalBlur;

uniform float brushBlurVal;

in vec2 texCoords;

void main(){
    //0.0003,	0.0004,	0.0007,	0.0012,	0.0019,	0.0029,	0.0044,	0.0064,	0.0090,	0.0124,	0.0166,	0.0216,	0.0274,	0.0337,	0.0404,	0.0470,	0.0532,	0.0587,	0.0629,	0.0655,	0.0665,	0.0655,	0.0629,	0.0587,	0.0532,	0.0470,	0.0404,	0.0337,	0.0274,	0.0216,	0.0166,	0.0124,	0.0090,	0.0064,	0.0044,	0.0029,	0.0019,	0.0012,	0.0007,	0.0004,	0.0003
    color = vec4(0.0);
    
    const float array[3] = float[3](2.5, 7.0, 1.5);


    vec2 centerTexCoords = texCoords;
    float pixelSize = 0.005;

    for(int i = 0; i < kernel_sizeF; i++) {
        if(i < 60){
            if(isRenderVerticalBlur == 0)
                color += texture(inputTexture, centerTexCoords + vec2(pixelSize*(i - (kernel_sizeF / 2)),0)) * oneDimensionalGaussF1[i];
            else
                color += texture(inputTexture, centerTexCoords + vec2(0,pixelSize*(i - (kernel_sizeF / 2)))) * oneDimensionalGaussF1[i];
        }
        else{
             if(isRenderVerticalBlur == 0)
                color += texture(inputTexture, centerTexCoords + vec2(pixelSize*(i - (kernel_sizeF / 2)),0)) * oneDimensionalGaussF2[i-60];
            else
                color += texture(inputTexture, centerTexCoords + vec2(0,pixelSize*(i - (kernel_sizeF / 2)))) * oneDimensionalGaussF2[i-60];
        }

    }
    
}