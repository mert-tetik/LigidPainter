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

glm::vec3 Mesh::getCenterPosition(){
    glm::vec3 center = glm::vec3(0.f);
    for (const Vertex& vertex : this->vertices) {
        center += vertex.Position;
    }

    if(center.x == 0.f)
        center.x += 0.00001f;
    if(center.y == 0.f)
        center.y += 0.00001f;
    if(center.z == 0.f)
        center.z += 0.00001f;

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
    const int displayRes = 256;
    
    if(this->displayingTxtr.ID == 0)
        this->displayingTxtr = Texture(nullptr, displayRes, displayRes, GL_LINEAR);
    else
        this->displayingTxtr.update(nullptr, displayRes, displayRes, GL_LINEAR);

    this->displayingTxtr.title = "MeshDisplayingTxtr";

    Framebuffer FBO = Framebuffer(this->displayingTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)));

    glm::vec3 meshCenter = this->getCenterPosition();
    
    glm::vec3 meshMostFar = this->getMostDistantPointFromAPoint(meshCenter);

    glm::vec3 camPos = glm::vec3(meshCenter.x - 4.f, meshCenter.y, meshCenter.z);

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(   
                                    camPos, 
                                    meshCenter, 
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
    
    FBO.bind();

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_CULL_FACE);

    //Use the 3D model rendering shader
    ShaderSystem::solidShadingShader().use();

    //Throw the camera data to the shader
    ShaderSystem::solidShadingShader().setMat4("projection", projectionMatrix);
    ShaderSystem::solidShadingShader().setMat4("view", view);
    glm::mat4 modelMat = glm::mat4(1.f);
    
    modelMat = glm::scale(modelMat, glm::vec3(1.f / glm::distance(meshMostFar, this->getCenterPosition())));

    ShaderSystem::solidShadingShader().setMat4("modelMatrix", modelMat);
    
    ShaderSystem::solidShadingShader().setVec3("viewPos", camPos);
    
    //Draw the sphere
    this->Draw(false);
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();    
    
    FBO.deleteBuffers(false, true);
}

void Mesh::generateUVMask(){
    const int resolution = 1024;
    
    this->uvMask = Texture(nullptr, resolution, resolution, GL_NEAREST);

    Framebuffer FBO = Framebuffer(this->uvMask, GL_TEXTURE_2D);
    glViewport(0, 0, resolution, resolution);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::uvMaskShader().use();
    ShaderSystem::uvMaskShader().setVec4("color", glm::vec4(1.));
    ShaderSystem::uvMaskShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f));

    this->Draw(false);

    Settings::defaultFramebuffer()->FBO.bind();
    FBO.deleteBuffers(false, false);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName)
{
    this->albedo = Texture(nullptr, 1024, 1024, GL_LINEAR);
    this->roughness = Texture(nullptr, 1024, 1024, GL_LINEAR);
    this->metallic = Texture(nullptr, 1024, 1024, GL_LINEAR);
    this->normalMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
    this->heightMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
    this->ambientOcclusion = Texture(nullptr, 1024, 1024, GL_LINEAR);

    this->vertices = vertices;
    this->indices = indices;
    this->materialName = materialName;

    setupMesh();

    generateUVMask();

    generateDisplayingTexture();
}

// Render the mesh
void Mesh::Draw(bool displayWireframe) 
{
    // draw mesh
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    // Wireframe  
    if(displayWireframe){
        glDepthFunc(GL_LESS);
        ShaderSystem::tdModelShader().setMat4("modelMatrix", glm::scale(getScene()->transformMatrix, glm::vec3(1.0015f)));
        ShaderSystem::tdModelShader().setInt("wireframeMode", 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ShaderSystem::tdModelShader().setMat4("modelMatrix", getScene()->transformMatrix);
        glDepthFunc(GL_LEQUAL);
        ShaderSystem::tdModelShader().setInt("wireframeMode", 0);
    }

    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    Settings::defaultFramebuffer()->FBO.bind();

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
}