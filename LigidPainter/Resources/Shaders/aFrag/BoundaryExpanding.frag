#version 400 core

in vec2 TexCoords;

uniform int renderingState; //0 = render the white uv texture , 1 = render the result
uniform sampler2D whiteUVTexture;
uniform sampler2D originalTexture;

out vec4 color;

void main(){
    //TODO GET PIXEL SIZE
    float pixel = 0.001;
    int radius = 10;

    //Paint around
    if(texture(whiteUVTexture,TexCoords).r < 0.95){
        for (int i = 0; i < radius; i++){
            if(texture(whiteUVTexture,vec2(TexCoords.x - pixel * (radius + 1), TexCoords.y)).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x - pixel * (radius + 1),TexCoords.y)));
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x + pixel * (radius + 1), TexCoords.y)).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x + pixel * (radius + 1), TexCoords.y)));
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y - pixel * (radius + 1))).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y - pixel * (radius + 1))));
            }
            else if(texture(whiteUVTexture,vec2(TexCoords.x, TexCoords.y + pixel * (radius + 1))).r > 0.95){
                color = vec4(texture(originalTexture,vec2(TexCoords.x, TexCoords.y + pixel * (radius + 1))));
            }
        }
    }
    else
        color = vec4(texture(originalTexture,TexCoords));
}