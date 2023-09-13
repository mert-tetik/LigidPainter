/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

static void initTexture(unsigned int &txtr,int textureRes){
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&txtr);
    glBindTexture(GL_TEXTURE_2D,txtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

glm::vec3 Mesh::getCenterPosition(){
    glm::vec3 center;
    for (const Vertex& vertex : this->vertices) {
        center += vertex.Position;
    }

    center /= static_cast<float>(vertices.size());

    return center;
}

glm::vec3 Mesh::getMostDistantPointFromAPoint(glm::vec3 point){
    glm::vec3 pos = point;
    for (const Vertex& vertex : this->vertices) {
        if(glm::distance(vertex.Position, point) > glm::distance(pos, point))
            pos = vertex.Position;
    }

    return pos;
}

float Mesh::getBiggestFloatValue(glm::vec3 origin){
    float big = 0.f;
    for (size_t vi = 0; vi < this->vertices.size(); vi++)
    {
        if(big < abs(this->vertices[vi].Position.x - origin.x))
            big = abs(this->vertices[vi].Position.x - origin.x);
        
        if(big < abs(this->vertices[vi].Position.y - origin.y))
            big = abs(this->vertices[vi].Position.y - origin.y);
        
        if(big < abs(this->vertices[vi].Position.z - origin.z))
            big = abs(this->vertices[vi].Position.z - origin.z);
    }

    return big;
}

void Mesh::generateDisplayingTexture(){
    if(this->displayingTxtr == 0)
        glGenTextures(1, &this->displayingTxtr);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->displayingTxtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    //Resolution of the material displaying texture
    const int displayRes = 256;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes, displayRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int FBO;

    //Capturing framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
    //Render buffer to render the sphere 3D model (for depth testing)
    unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);

    glDisable(GL_CULL_FACE);
	
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayRes, displayRes);
	
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    //Bind the displaying texture of the material to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->displayingTxtr, 0);

    glm::vec3 meshCenter = this->getCenterPosition();
    
    glm::vec3 meshMostFar = this->getMostDistantPointFromAPoint(meshCenter);

    glm::vec3 camPos = glm::vec3(0.f + 15.f, 0.f, 0.f);

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(   
                                    camPos, 
                                    glm::vec3(0.f), 
                                    glm::vec3(0.0, 1.0, 0.0)
                                );
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    0.1f,  //Near 
                                                    1000.1f    //Far
                                                );
    
    //!Update the material displaying texture
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::solidShadingShader().use();

    //Throw the camera data to the shader
    ShaderSystem::solidShadingShader().setMat4("projection", projectionMatrix);
    ShaderSystem::solidShadingShader().setMat4("view", view);
    glm::mat4 modelMat = glm::mat4(1.f);
    modelMat = glm::scale(modelMat, glm::vec3(1.f / glm::distance(meshMostFar, meshCenter)));
    std::cout << "DIN " << glm::to_string(meshCenter) << ' ' << glm::to_string(meshMostFar) << ' ' << glm::distance(meshMostFar, meshCenter) << std::endl;

    ShaderSystem::solidShadingShader().setMat4("modelMatrix", modelMat);
    
    ShaderSystem::solidShadingShader().setVec3("viewPos", camPos);
    
    //Draw the sphere
    this->Draw();
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    //Set the OpenGL viewport to default
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
}

void Mesh::generateUVMask(){
    const int resolution = 1024;
    
    initTexture(this->uvMask, resolution);

    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->uvMask, 0);
    glViewport(0, 0, resolution, resolution);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::uvMaskShader().use();
    ShaderSystem::uvMaskShader().setVec4("color", glm::vec4(1.));
    ShaderSystem::uvMaskShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f));

    this->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName)
{
    initTexture(albedo.ID, 1024);
    initTexture(roughness.ID, 1024);
    initTexture(metallic.ID, 1024);
    initTexture(normalMap.ID, 1024);
    initTexture(heightMap.ID, 1024);
    initTexture(ambientOcclusion.ID, 1024);

    this->vertices = vertices;
    this->indices = indices;
    this->materialName = materialName;

    setupMesh();

    generateUVMask();

    generateDisplayingTexture();
}

// Render the mesh
void Mesh::Draw() 
{
    // draw mesh
    glBindBuffer(1,VBO);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    
    glBindBuffer(1, 0);
    glBindVertexArray(0);
    
    // always good practice to set everything back to defaults once configured.
}

void Mesh::processHeightMap(){

    std::vector<Vertex> newVertArray = this->vertices;

    char* txtr = new char[this->heightMap.getResolution().x * this->heightMap.getResolution().y * 4];
    this->heightMap.getData(txtr);
    if(Settings::properties()->useHeightMap){
        for (size_t i = 0; i < this->vertices.size(); i++)
        {
            glm::vec2 uv = this->vertices[i].TexCoords;
            glm::ivec2 pixelDest = this->vertices[i].TexCoords * glm::vec2(this->heightMap.getResolution()); 
            
            int pixelIndex = (pixelDest.y * this->heightMap.getResolution().x + pixelDest.x) * 4; // Each pixel has 4 components (R, G, B, A)
            if(pixelIndex < this->heightMap.getResolution().x * this->heightMap.getResolution().y * 4){
                char red = txtr[pixelIndex];
                char green = txtr[pixelIndex + 1];
                char blue = txtr[pixelIndex + 2];
                char alpha = txtr[pixelIndex + 3]; 
                
                newVertArray[i].Position += glm::vec3(((float)red / 127.f - 0.5) * Settings::properties()->heightMapStrength) * newVertArray[i].Normal;
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, newVertArray.size() * sizeof(Vertex), &newVertArray[0], GL_STATIC_DRAW);  

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete[] txtr;
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
    if(!vertices.size())
        vertices.push_back(Vertex());

    if(!indices.size())
        indices.push_back(0);

    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    // vertex normals
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)(14 * sizeof(float)));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(18 * sizeof(float)));
}