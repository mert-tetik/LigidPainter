#version 400 core

out vec4 fragColor;

uniform int emptyFlag;

void main(){
    fragColor.r = gl_PrimitiveID + 1;
    fragColor.g = 1.;
    fragColor.b = 1.;
    fragColor.a = 1.;
    
    if(emptyFlag == 1)
        fragColor.r = 0;
    //gl_PrimitiveID / 1000
}