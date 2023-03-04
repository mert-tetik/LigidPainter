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
        color = vec4(iconColor.rgb, texture2D(icon,TexCoords).r-0.02); //Revert colors while rendering mask icons
        gl_FragDepth = 0.05;

    }
    else if(isMaskIcon == 0){
        vec2 uv = TexCoords;
        uv.y = 1. - uv.y;
        if((1.-texture2D(icon,uv).r) < 0.05)
            gl_FragDepth = 1.;
        else 
            gl_FragDepth = gl_FragCoord.z;
        color = vec4(mix(iconColor.rgb,iconColorHover.rgb,iconMixVal), (1.0 - texture2D(icon,uv).r) * iconColor.a); 
    }
}