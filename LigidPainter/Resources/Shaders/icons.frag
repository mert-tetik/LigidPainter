#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D icon;
uniform vec3 iconColor;
uniform vec3 iconColorHover;
uniform float iconMixVal;

uniform int isMaskIcon; //Revert colors while rendering mask icons

void main(){
    if(isMaskIcon == 1){
        color = vec4(iconColor, texture2D(icon,TexCoords).r-0.02); //Revert colors while rendering mask icons
    }
    else if(isMaskIcon == 0){
        color = vec4(mix(iconColor,iconColorHover,iconMixVal), 1.0 - texture2D(icon,TexCoords).r); 
    }
}