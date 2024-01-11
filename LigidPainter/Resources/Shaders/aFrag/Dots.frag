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

out vec4 fragColor;

uniform vec3 pos;
uniform vec2 scale;

uniform vec2 dotPos;
uniform float scroll;

float applyThreshold(float value, float threshold) {
    if (value >= threshold){
        while(value > threshold){
            value -= threshold;
        }

        if(value == 0.)
            value+=0.1; 

        return value; 
    }
    else
        return value;
}

void main(){
    float m = 0.;

    float dotSize = 1;
    float dotGap = 20;

    vec2 UV = TexCoords * scale + pos.xy;

    UV.x -= dotPos.x;
    UV.y += dotPos.y;
    
    float tScr = applyThreshold(scroll,5);

    if(tScr == 0.)
        tScr += 0.1;

    UV /= tScr;

    vec2 tUV = vec2(applyThreshold(UV.x, dotGap),applyThreshold(UV.y, dotGap));

    if(tUV.x <= dotSize && tUV.y <= dotSize)
        m = 1 / 1.5;

    fragColor = vec4(m);
}