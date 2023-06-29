/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    2D vertex shader (coordinates determined by the uniform variables)

    Render square VBO in the range of 0 - 1 using that shader

    Example to render a square in the middle of the screen :

    std::vector<float> boxVertices = { 
        1.0f,   1.0f, 0.0f      ,      1,0,         // top right
        1.0f,  -1.0f, 0.0f      ,      1,1,         // bottom right
       -1.0f,   1.0f, 0.0f      ,      0,0,         // top left 
        
        1.0f,  -1.0f, 0.0f     ,       1,1,         // bottom right
       -1.0f,  -1.0f, 0.0f     ,       0,1,         // bottom left
       -1.0f,   1.0f, 0.0f     ,       0,0          // top left
    };
    
    Orthographic projection matrix (retrieved from the CPU)
    projection = L : 0 , R : 1920 , T : 0 , B : 1080;

    Position value of the box
    pos = 1920/2 , 1080/2 , 1;

    Scale value of the box
    scale = 100 , 100;
*/



//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core




//---------------- LAYOUTS ----------------

// Specify the layout of the vertex attribute 'aPos'
layout(location = 0) in vec3 aPos;
// 'aPos' is a 3-component vector representing the position attribute of each vertex.
// The 'location = 0' qualifier assigns this attribute to location 0 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.

// Specify the layout of the vertex attribute 'aTexCoords'
layout(location = 1) in vec2 aTexCoords;
// 'aTexCoords' is a 2-component vector representing the texture coordinate attribute of each vertex.
// The 'location = 1' qualifier assigns this attribute to location 1 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.
// Texture coordinates define how textures are mapped onto the vertices of a shape, allowing for texture mapping effects.



//---------------- UNFIFORMS ----------------

//Orthographic projection matrix (retrieved from the CPU)
uniform mat4 projection;
uniform mat4 projectedPosProjection;

//Position value of the box
uniform vec3 pos;

//Scale value of the box
uniform vec2 scale;




//---------------- OUTPUT ----------------

//Position data of the vertex (send to the fragment shader)
out vec3 fragPos;

//Texture coordinate data of the vertex (send to the fragment shader)
out vec2 TexCoords;

//Scale value of the vertex (send to the fragment shader)
out vec2 uniScale;


out vec4 ProjectedPos;







//---------------- MAIN ----------------

void setOutputs(){
    uniScale = scale;
    TexCoords = aTexCoords;
    fragPos = aPos;
}

void main() {
    
    //Equate the vertex data to the output values
    setOutputs();

    //Scale the position data of the vertex
    vec3 scaledPos = aPos * vec3(scale,1);

    //Move the position data of the vertex to the pos uniform 
    vec3 positionedPos = scaledPos + pos;

    ProjectedPos = projectedPosProjection * vec4(positionedPos, 1.0); 

    gl_Position = projection * vec4(positionedPos, 1.0);;
}