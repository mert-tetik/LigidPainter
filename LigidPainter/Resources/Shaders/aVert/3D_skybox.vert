/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    3D Skybox shader.

    Render skyboxes using this vertex shader.

*/



//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core


//---------------- LAYOUTS ----------------

// Specify the layout of the vertex attribute 'aPos'
layout(location = 0) in vec3 aPos;
// 'aPos' is a 3-component vector representing the position attribute of each vertex.
// The 'location = 0' qualifier assigns this attribute to location 0 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.



//---------------- UNIFORMS ----------------

//View matrix generated using the camera data
uniform mat4 view;

//Perspective projection matrix 
uniform mat4 projection;

//Skybox transform matrix
uniform mat4 transformMatrix;



//---------------- OUTPUT ----------------

//Send the position data of the cube's vertices to the fragment shader
out vec3 Pos;

void main(){
    
    //Send the vertex pos data to the fragment shader
    Pos = aPos;
    
    vec4 transformedPos = transformMatrix * vec4(Pos,1);
    
    mat4 viewUpperLeft3x3 = mat4(mat3(view));

    vec4 cubePos = projection * viewUpperLeft3x3  * vec4(-transformedPos);
    
    vec4 screenCoordinates = cubePos.xyww;

    gl_Position = screenCoordinates;
}