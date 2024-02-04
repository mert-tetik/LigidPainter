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
uniform float opacity;

uniform int depthToleranceMode = 0;

uniform vec3 mixClr = vec3(0.);

out vec4 fragColor;

void main(){
    fragColor = texture(txtr, TexCoords);
    fragColor.a *= opacity;

    if(mixClr.r != 0 && mixClr.g != 0 && mixClr.b != 0){
        fragColor.rgb = mix(fragColor.rgb, mixClr, 0.5);
    }

    gl_FragDepth = gl_FragCoord.z;

    if(depthToleranceMode == 1)
        gl_FragDepth = gl_FragCoord.z - 0.001;
}