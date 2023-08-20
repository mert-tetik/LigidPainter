#version 400 core

in vec2 TexCoords;
in vec3 Pos;

out vec4 fragColor;

void main(){
    vec3 clr = vec3(0);
    if(abs(Pos.x) > 0.)
        clr = vec3(0.9, 0.22, 0.33); 
    if(abs(Pos.y) > 0.)
        clr = vec3(0.55, 0.83, 0.65); 
    if(abs(Pos.z) > 0.)
        clr = vec3(0., 0.59, 1.); 
    fragColor = vec4(clr, 0.4);
}