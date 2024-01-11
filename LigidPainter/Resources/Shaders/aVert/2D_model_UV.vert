/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    2D model UV vertex shader

    Render 3D model's texture coordinates in 2D

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

// Specify the layout of the vertex attribute 'aNormal'
layout(location = 2) in vec3 aNormal;
// 'aNormal' is a 3-component vector representing the normal attribute of each vertex.
// The 'location = 2' qualifier assigns this attribute to location 2 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.
// Normals define the direction perpendicular to a surface, used for lighting calculations and shading.

// Specify the layout of the vertex attribute 'aTangent'
layout(location = 3) in vec3 aTangent;
// 'aTangent' is a 3-component vector representing the tangent attribute of each vertex.
// The 'location = 3' qualifier assigns this attribute to location 3 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.
// Tangents define the direction of the surface's tangent vector, used for normal mapping and advanced shading techniques.

// Specify the layout of the vertex attribute 'aBitangent'
layout(location = 4) in vec3 aBitangent;
// 'aBitangent' is a 3-component vector representing the bitangent attribute of each vertex.
// The 'location = 4' qualifier assigns this attribute to location 4 in the vertex shader.
// This layout qualifier is used to associate the attribute with a specific vertex attribute location.
// Bitangents (also known as binormals) define the direction perpendicular to both the normal and tangent vectors, used for normal mapping and advanced shading techniques.




//---------------- UNFIFORMS ----------------

//Orthographic projection matrix (retrieved from the CPU) (to render the texture coordinates)
uniform mat4 orthoProjection;

//Perspective projection matrix (to calculate the projected pos)
uniform mat4 perspectiveProjection;

//View matrix generated using the camera data (to calculate the projected pos)
uniform mat4 view;



//---------------- OUTPUTS ----------------

//Vertex data of the 3D model
out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec3 Tangent;
out vec3 Bitangent;
out vec4 ProjectedPos;




//---------------- MAIN ----------------

void setOutputs(){
    Tangent = aTangent;
    Bitangent = aBitangent;
    TexCoords = aTexCoords;
    Normal = aNormal;
    Pos = aPos;
}

void main() {

    //Send the vertex data to the fragment shader
    setOutputs();

    //Calculate the screen coordinates
    ProjectedPos = perspectiveProjection * view * vec4(Pos, 1.); 

    //Set the vertex position
    gl_Position = orthoProjection * vec4(TexCoords, 1.0, 1.0); 

}