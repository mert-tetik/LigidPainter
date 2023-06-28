//Is used to render the skybox

#version 400 core

in vec3 Pos;

uniform samplerCube skybox;
uniform float lod;

uniform vec3 bgColor; 
uniform float opacity; 

out vec4 color;

void main(){
    color = vec4(textureLod(skybox, Pos * - 1, lod).rgb * 1 , 1);
    color.rgb = mix(bgColor,color.rgb,opacity);
}