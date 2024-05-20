/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Used for rendering consecutive dozens of tiny dots.

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;
uniform int spriteI;

out vec4 fragColor;

void main(){
    vec2 uv = TexCoords;
    
    if(spriteI == 0){
        uv.x /= 10.; // 0.0 - 0.1 
    }
    
    if(spriteI == 1){
        uv.x /= 1.25; // 0.0 - 0.8 
        uv.x += 0.1; // 0.1 - 0.9
    }
    
    if(spriteI == 2){
        uv.x /= 10.; // 0.0 - 0.1 
        uv.x += 0.9; // 0.9 - 1.0
    }

    fragColor = texture(txtr, uv);

}