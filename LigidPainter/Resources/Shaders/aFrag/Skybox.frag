/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Skybox fragment shader.

    Is used to render the skybox

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Get the position data from the skybox vertex shader
in vec3 Pos;

//The skybox texture (is a cubemap) (used the prefiltered version for the bluring)
uniform samplerCube skybox;

//Lod value to blur the skybox
uniform float lod;

//Color value selected by the user in the displayer settings dialog
uniform vec3 bgColor; 

//Skybox opacity value selected by the user in the displayer settings dialog (used to mix the skybox with bgColor)
uniform float opacity; 

//Fragment shader output
out vec4 color;

// ---- gradient ----
uniform int gradient;
uniform float gradientOffset;

void main(){
    if(gradient == 0){
        color = vec4(
                        textureLod(skybox, Pos * - 1, lod).rgb, //Sybox value 
                        1 //Full opacity
                    );
        
        
        //Mix the skybox color value with the bg color value using the opacity uniform variable
        color.rgb = mix(
                            bgColor,
                            color.rgb,
                            opacity //Opacity = 1 : Just the background color
                        );
    }
    else if(gradient == 1){
        float grad;
        grad = ((Pos.y * gradientOffset) + 1.) / 2.;
        color.rgb = vec3(grad);
        color.a = 1.f;
    }
}