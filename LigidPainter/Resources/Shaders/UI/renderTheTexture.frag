#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform int isHover;
uniform int isPressed;

uniform sampler2D texture;

void main(){
    if(isHover == 0){
        if(isPressed == 0){
            //Default
            color = texture2D(texture,TexCoords);
        }
        else{ 
            //Pressed(negative)
            color = vec4(vec3(vec4(0.043,0.635,0.823,1) - texture2D(texture,TexCoords)),1);
        }
    }
    if(isHover == 1){
        if(isPressed == 0){
            //Hover (transparent)
            color = vec4(vec3(texture2D(texture,TexCoords)),0.5);
        }
        else{ 
            //Pressed + hover (transparent + negative)
            color = vec4(vec3(vec4(0.043,0.635,0.823,1) - texture2D(texture,TexCoords)),0.5);
        }
    }
}