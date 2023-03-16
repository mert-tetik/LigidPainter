#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D icon;
uniform vec4 iconColor;
uniform vec4 iconColorHover;
uniform float iconMixVal;

uniform int isMaskIcon; //Revert colors while rendering mask icons

void main(){
    if(isMaskIcon == 1){
        color = vec4(iconColor.rgb, texture(icon,TexCoords).r-0.02); //Revert colors while rendering mask icons
    }
    else if(isMaskIcon == 0){
        color = vec4(mix(iconColor.rgb,iconColorHover.rgb,iconMixVal), (1.0 - texture(icon,TexCoords).r) * iconColor.a); 
    }
}