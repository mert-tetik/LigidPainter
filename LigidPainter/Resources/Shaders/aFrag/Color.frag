#version 400 core

uniform vec4 color;

out vec4 fragColor;

uniform float depthToleranceValue;

void main(){
    fragColor = color;

    gl_FragDepth = gl_FragCoord.z - depthToleranceValue;
}