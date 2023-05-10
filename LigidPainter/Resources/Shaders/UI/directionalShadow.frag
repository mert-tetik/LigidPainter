#version 330 core


out vec4 fragColor;
in vec2 TexCoords;


void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = TexCoords;
    float r = smoothstep(0.,1.,uv.x*5.+0.7);


    // Output to screen
    fragColor = vec4(vec3(r),1.0);
}