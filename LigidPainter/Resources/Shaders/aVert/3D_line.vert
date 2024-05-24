/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

uniform vec3[100] offset_pos;

out vec3 Pos;
out vec4 ProjectedPos;

void main() {

    //Calculate the screen coordinates
    Pos = aPos + offset_pos[gl_InstanceID];
    ProjectedPos = projection * view * vec4(aPos.xyz + offset_pos[gl_InstanceID], 1.); 
    
    //Set the vertex position
    gl_Position = ProjectedPos;
}