#version 330 core

in vec2 TexCoords;

uniform int renderingState; //0 = render the white uv texture , 1 = render the result
uniform sampler2D whiteUVTexture;
uniform sampler2D originalTexture;

out vec4 color;

void main(){
    if(renderingState == 0){
        color = vec4(1);
    }
    else if(renderingState == 1){
        //Paint around
        if(texture(whiteUVTexture,vec2(TexCoords.x - 0.004,TexCoords.y)).r > 0.95 && texture(whiteUVTexture,TexCoords).r < 0.95){
           color = vec4(texture(originalTexture,vec2(TexCoords.x - 0.004,TexCoords.y)));
        }
        else if(texture(whiteUVTexture,vec2(TexCoords.x + 0.004,TexCoords.y)).r > 0.95 && texture(whiteUVTexture,TexCoords).r < 0.95){
           color = vec4(texture(originalTexture,vec2(TexCoords.x + 0.004,TexCoords.y)));
        }
        else if(texture(whiteUVTexture,vec2(TexCoords.x,TexCoords.y - 0.004)).r > 0.95 && texture(whiteUVTexture,TexCoords).r < 0.95){
           color = vec4(texture(originalTexture,vec2(TexCoords.x ,TexCoords.y - 0.004)));
        }
        else if(texture(whiteUVTexture,vec2(TexCoords.x,TexCoords.y + 0.004)).r > 0.95 && texture(whiteUVTexture,TexCoords).r < 0.95){
           color = vec4(texture(originalTexture,vec2(TexCoords.x ,TexCoords.y + 0.004)));
        }
        else{
           color = vec4(texture(originalTexture,TexCoords));
        }
    }
}