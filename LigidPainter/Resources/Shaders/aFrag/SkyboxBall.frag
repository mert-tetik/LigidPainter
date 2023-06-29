/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Skybox ball shader.

    Used to render a skybox ball. 

    Reflects the skybox from a surface. (Used with a sphere in the LigidPainter)

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//3D Model single vertex data
struct VertexData{
    vec2 TexCoords;
    vec3 Normal;
    vec3 Pos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 ProjectedPos;
};

//Get the vertex data from the vertex shader
in VertexData vertexData;

//Skybox texture (cubemap)
uniform samplerCube skybox;

//The fragment shader output
out vec4 fragColor;

vec3 getReflection(){

    //Normal vector of the vertex
    vec3 N = vertexData.Normal;

    //Idk might be useful 
    //! BUT IS NOT RN
    //N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space

    return texture(skybox, N).rgb ;
}

void main() {
    fragColor = vec4(getReflection(),1.);
}