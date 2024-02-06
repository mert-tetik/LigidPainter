#version 400 core

uniform vec4 color;

out vec4 fragColor;

uniform int depthToleranceMode = 0;

void main(){
    fragColor = color;

    gl_FragDepth = gl_FragCoord.z;
    
    if(depthToleranceMode == 1)
        gl_FragDepth = gl_FragCoord.z - 0.003;
}