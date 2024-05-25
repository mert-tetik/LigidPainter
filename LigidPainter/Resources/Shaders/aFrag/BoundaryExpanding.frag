#version 400 core

in vec2 TexCoords;

uniform sampler2D whiteUVTexture;
uniform sampler2D originalTexture;
uniform int resolution;

out vec4 color;

void main(){
    //TODO GET PIXEL SIZE
    float pixel = 1. / resolution;
    const float uv_radius = 0.01;
    
    int radius = int(float(uv_radius / pixel));

    //Paint around
    if(texture(whiteUVTexture,TexCoords).r != 1.){
        for (int i = 0; i < radius; i++){
            /* Rook movement*/
            if(texture(whiteUVTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y)).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y)));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y)).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y)));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y - pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y - pixel * (i))));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y + pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y + pixel * (i))));
                break;
            }
            
            
            /* Bishop movement*/
            else if(texture(whiteUVTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y - pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y - pixel * (i))));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y + pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y + pixel * (i))));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y - pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y - pixel * (i))));
                break;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y + pixel * (i))).r == 1.){
                color = vec4(texture(originalTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y + pixel * (i))));
                break;
            }
            
            else
                color = vec4(texture(originalTexture,TexCoords));
        }
    }
    else
        color = vec4(texture(originalTexture,TexCoords));
}