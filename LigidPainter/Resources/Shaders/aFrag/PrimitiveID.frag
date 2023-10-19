#version 400 core

out vec4 fragColor;

void main(){
    fragColor.r = gl_PrimitiveID;
    fragColor.g = 1.;
    fragColor.b = 1.;
    fragColor.a = 1.;
    //gl_PrimitiveID / 1000
}