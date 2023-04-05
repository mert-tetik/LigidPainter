#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform int isHover;
uniform int isPressed;

uniform int subSelected;

uniform int isMask;
uniform int maskUseColor;

uniform sampler2D txtr;

void main(){
    if(isMask == 0){
        vec3 selectionColor;
        if(subSelected == 0)
            selectionColor = vec3(0.043,0.635,0.823);
        if(subSelected == 1)
            selectionColor = vec3(1) - vec3(0.043,0.635,0.823);

          if(isHover == 0){
            if(isPressed == 0){
                //Default
                color = texture(txtr,TexCoords);
            }
            else{ 
                const float cornerThickness = 0.02; 
                //Pressed(negative)
                if(TexCoords.x < cornerThickness || TexCoords.y < cornerThickness || TexCoords.x > 1.0-cornerThickness || TexCoords.y > 1.0-cornerThickness){
                    if(texture(txtr,TexCoords).r == texture(txtr,TexCoords).g && texture(txtr,TexCoords).r == texture(txtr,TexCoords).b)
                        color = vec4(selectionColor,1);
                    else
                        color = vec4(vec3(vec4(selectionColor,1) - texture(txtr,TexCoords)),1*texture(txtr,TexCoords).a);
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
                    if(texture(txtr,TexCoords).r == texture(txtr,TexCoords).g && texture(txtr,TexCoords).r == texture(txtr,TexCoords).b)
                        color = vec4(selectionColor,1);
                    else
                        color = vec4(vec3(vec4(selectionColor,1) - texture(txtr,TexCoords)),0.5*texture(txtr,TexCoords).a);
                }
                else{
                    color = vec4(texture(txtr,TexCoords).rgb,0.5*texture(txtr,TexCoords).a);
                }
            }
        }
    }
    else{
        if(isPressed == 1){
            float maskTxtr = max(texture(txtr, TexCoords),0.0).r;

            const float thickness = 0.03;
            
            float maskTxtrLeft; 
            float maskTxtrRight; 
            float maskTxtrTop; 
            float maskTxtrBot; 
            if(maskUseColor == 1){
                maskTxtrLeft = max(texture(txtr, TexCoords - vec2(thickness,0)),0.0).a;
                maskTxtrRight = max(texture(txtr, TexCoords + vec2(thickness,0)),0.0).a;
                maskTxtrTop = max(texture(txtr, TexCoords - vec2(0,thickness)),0.0).a;
                maskTxtrBot = max(texture(txtr, TexCoords + vec2(0,thickness)),0.0).a;
            }
            else{
                maskTxtrLeft = max(texture(txtr, TexCoords - vec2(thickness,0)),0.0).r;
                maskTxtrRight = max(texture(txtr, TexCoords + vec2(thickness,0)),0.0).r;
                maskTxtrTop = max(texture(txtr, TexCoords - vec2(0,thickness)),0.0).r;
                maskTxtrBot = max(texture(txtr, TexCoords + vec2(0,thickness)),0.0).r;
            }

            float maskResult = 0;

            if(maskTxtr > 0.02){
               if(maskTxtrLeft < 0.02 || maskTxtrRight < 0.02 || maskTxtrTop < 0.02 || maskTxtrBot < 0.02){
                  maskResult = 1.0;
               }
            }
            vec3 res = texture(txtr,TexCoords).rgb;
            if(maskResult == 1.0)
                res = vec3(0.043,0.635,0.823);

            if(maskUseColor == 1)
                color = vec4(res,texture(txtr,TexCoords).a); 
            else
                color = vec4(res,texture(txtr,TexCoords).r); 
        }
        else{
            if(maskUseColor == 1)
                color = vec4(texture(txtr,TexCoords)); 
            else
                color = vec4(texture(txtr,TexCoords).r); 
        }
    }
}   