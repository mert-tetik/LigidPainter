#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform int isHover;
uniform int isPressed;

uniform sampler2D txtr;

void main(){
    if(isHover == 0){
        if(isPressed == 0){
            //Default
            color = texture(txtr,TexCoords);
        }
        else{ 
            const float cornerThickness = 0.02; 
            //Pressed(negative)
            if(TexCoords.x < cornerThickness || TexCoords.y < cornerThickness || TexCoords.x > 1.0-cornerThickness || TexCoords.y > 1.0-cornerThickness){
                color = vec4(vec3(vec4(0.043,0.635,0.823,1) - texture(txtr,TexCoords)),1*texture(txtr,TexCoords).a);
            }
            else{
                color = vec4(texture(txtr,TexCoords).rgb,1*texture(txtr,TexCoords).a);
            }
        }
    }
    if(isHover == 1){
        if(isPressed == 0){
            //Hover (transparent)
            color = vec4(vec3(texture(txtr,TexCoords)),0.5*texture(txtr,TexCoords).a);
        }
        else{ 
            const float cornerThickness = 0.04; 
            //Pressed + hover (transparent + negative)
            if(TexCoords.x < cornerThickness || TexCoords.y < cornerThickness || TexCoords.x > 1.0-cornerThickness || TexCoords.y > 1.0-cornerThickness){
                color = vec4(vec3(vec4(0.043,0.635,0.823,1) - texture(txtr,TexCoords)),0.5*texture(txtr,TexCoords).a);
            }
            else{
                color = vec4(texture(txtr,TexCoords).rgb,0.5*texture(txtr,TexCoords).a);
            }
        }
    }
}