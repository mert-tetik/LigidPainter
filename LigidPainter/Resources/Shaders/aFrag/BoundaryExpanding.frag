#version 400 core

in vec2 TexCoords;

uniform sampler2D whiteUVTexture;
uniform sampler2D originalTexture;

out vec4 color;

void main(){
    //TODO GET PIXEL SIZE
    float pixel = 0.000025;
    int radius = 200;

    //Paint around
    if(texture(whiteUVTexture,TexCoords).r < 0.95){
        for (int i = 0; i < radius; i++){
            bool f = false;
            if(texture(whiteUVTexture,vec2(TexCoords.x - pixel * (i), TexCoords.y)).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x - pixel * (i),TexCoords.y)));
                f = true;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y)).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x + pixel * (i), TexCoords.y)));
                f = true;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y - pixel * (i))).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y - pixel * (i))));
                f = true;
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y + pixel * (i))).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y + pixel * (i))));
                f = true;
            }
            else if(!f)
                color = vec4(texture(originalTexture,TexCoords));
        }
    }
    else
        color = vec4(texture(originalTexture,TexCoords));
}