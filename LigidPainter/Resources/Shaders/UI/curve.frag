#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform float w;
uniform float h;
uniform vec4 lineColor;

void main(){
        // Normalized pixel coordinates (from -0.5 to 0.5 (origo in center))
    vec2 iResolution = vec2(1);
      
    vec2 uv = TexCoords/iResolution.xy - 0.5;
    
    float pi = 3.1415;
    float freq = 1.;
    float amp = 0.5;

    // Calculate y-pos out of x-pos
    float y = sin(uv.x * pi * freq + 3.1) * amp;
    float yr = 1.-abs(uv.y - y) * (h*300);
    // Output to screen
    if(yr<0.5)
        gl_FragDepth = 1;
    else
        gl_FragDepth = 0.41;

    color = vec4(vec3(lineColor),yr);
}