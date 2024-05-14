/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h> // holds all OpenGL type declarations
#include <LigidGL/LigidGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include "3D/Mesh/Mesh.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"

std::mutex mesh_mutex;

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
        this->displayingTxtr = Texture((char*)nullptr, displayRes, displayRes, GL_LINEAR);
    else
        this->displayingTxtr.update((char*)nullptr, displayRes, displayRes, GL_LINEAR);

    this->displayingTxtr.title = "MeshDisplayingTxtr";

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
    
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->displayingTxtr, this->displayingTxtr.getResolution(), "");

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
    

    this->Draw("Mesh::generateDisplayingTexture");
    
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}

void Mesh::generateUVMask(){
    
    const int resolution = 2024;
    
    this->uvMask = Texture((char*)nullptr, resolution, resolution, GL_LINEAR);

    Framebuffer FBO = FBOPOOL::requestFBO(this->uvMask, "Generating mesh uv mask");

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::uvMaskShader().use();
    ShaderSystem::uvMaskShader().setVec4("color", glm::vec4(1.));
    ShaderSystem::uvMaskShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f));
    ShaderSystem::uvMaskShader().setFloat("depthToleranceValue", 0);

    this->Draw("Mesh::generateUVMask");

    FBOPOOL::releaseFBO(FBO);
    ShaderUTIL::release_bound_shader();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName, bool initTxtrs)
{
    std::cout << "" << std::endl;
    if(!vertices.size())
        vertices.push_back(Vertex());

    if(!indices.size())
        indices.push_back(0);
    
    this->vertices = vertices;
    this->indices = indices;
    this->materialName = materialName;

    if(initTxtrs){
        this->material_channels.albedo = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->material_channels.roughness = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->material_channels.metallic = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->material_channels.normalMap = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->material_channels.heightMap = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->material_channels.ambientOcclusion = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);
        this->objectIDs = Texture((char*)nullptr, 1024, 1024, GL_LINEAR, GL_RG, GL_RG32F);
        
        this->face_selection_data.modelPrimitives = Texture((char*)nullptr, 1024, 1024, GL_NEAREST, GL_RED, GL_R32F);
        char whitePx[] = {127, 127, 127, 127};
        this->face_selection_data.meshMask = Texture(whitePx, 1, 1, GL_NEAREST);


        this->meshPosTxtr = Texture((char*)nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);
        this->meshNormalTxtr = Texture((char*)nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);

        if(indices.size()){
            this->face_selection_data.meshMask_display_txtr = Texture((char*)nullptr, 512, 512);
            this->face_selection_data.meshMask_display_txtr.proceduralProps.proceduralID = 24;
            this->face_selection_data.meshMask_display_txtr.proceduralProps.proceduralnverted = false;
            this->face_selection_data.meshMask_display_txtr.generateProceduralDisplayingTexture(512, 1);
            
            this->face_selection_data.selectedPrimitiveIDs.resize(this->indices.size() / 3);
            std::fill(this->face_selection_data.selectedPrimitiveIDs.begin(), this->face_selection_data.selectedPrimitiveIDs.end(), 0);

            int resolution = std::ceil(sqrt(this->face_selection_data.selectedPrimitiveIDs.size()));
            this->face_selection_data.selectedFaces = Texture(nullptr, nullptr, false, resolution, resolution, GL_NEAREST, GL_RED, GL_R8, 0, GL_TEXTURE_2D);
        }
    }

    generateUVMask();

    generateDisplayingTexture();
}


void Mesh::getPosNormalOverPoint(glm::vec2 pointPos, float*& posData, float*& normalData, bool readNormal){
        
    const unsigned int resolution = this->meshPosTxtr.getResolution().x; 

    pointPos.x = UTIL::new_value_range(pointPos.x, 0, getContext()->windowScale.x, 0, resolution);
    pointPos.y = UTIL::new_value_range(pointPos.y, 0, getContext()->windowScale.y, 0, resolution);

    {
        Framebuffer FBO = FBOPOOL::requestFBO(this->meshPosTxtr, "Mesh::getPosNormalOverPoint : position");

        glReadPixels(
                        pointPos.x, 
                        pointPos.y, 
                        1, 
                        1,
                        GL_RGBA,
                        GL_FLOAT,
                        posData
                    );

        FBOPOOL::releaseFBO(FBO);
    }
    
    if(readNormal)
    {
        Framebuffer FBO = FBOPOOL::requestFBO(this->meshNormalTxtr, "Mesh::getPosNormalOverPoint : normal");

        glReadPixels(
                        pointPos.x, 
                        pointPos.y, 
                        1, 
                        1,
                        GL_RGBA,
                        GL_FLOAT,
                        normalData
                    );

        FBOPOOL::releaseFBO(FBO);
    }
    
    normalData[0] = normalData[0] * 2.f - 1.f;
    normalData[1] = normalData[1] * 2.f - 1.f;
    normalData[2] = normalData[2] * 2.f - 1.f;
    
    posData[0] = posData[0] * 2.f - 1.f;
    posData[1] = posData[1] * 2.f - 1.f;
    posData[2] = posData[2] * 2.f - 1.f;
}

void Mesh::updatePosNormalTexture(){
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    unsigned int resolution = this->meshPosTxtr.getResolution().x; 
    
    for (size_t i = 0; i < 2; i++)
    {
        Texture txtr = this->meshPosTxtr;
        if(i == 1)
            txtr = this->meshNormalTxtr;

        //Bind the texture (Painter class public member variable)
        Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(txtr, txtr.getResolution(), "Mesh::updatePosNormalTexture");

        //Clear the texture
        glViewport(0, 0, resolution, resolution);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use the depth 3D shader
        ShaderSystem::renderModelData().use();
        ShaderSystem::renderModelData().setMat4("view", getScene()->camera.viewMatrix);
        ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
        ShaderSystem::renderModelData().setInt("state", i + 1);

        ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::renderModelData(), *this);

        //Draw the selected mesh in 3D
        ShaderSystem::renderModelData().setInt("primitiveCount", this->indices.size() / 3);
        this->Draw("Mesh::updatePosNormalTexture");
        
        GL::releaseBoundTextures("Mesh : updatePosNormalTexture");
        FBOPOOL::releaseFBO(FBO);
        ShaderUTIL::release_bound_shader();
    }
    
    ShaderUTIL::release_bound_shader();

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
}

ThreeDPoint Mesh::getCurrentPosNormalDataOverCursor(){
    // Retrieve the position value of the 3D model here
    float* posData = new float[4]; 
    float* normalData = new float[4]; 

    // Get the position value of the 3D model wherever it got clicked
    glm::vec2 currentCursorPos = glm::vec2(Mouse::cursorPos()->x, getContext()->windowScale.y - Mouse::cursorPos()->y);
    this->getPosNormalOverPoint(currentCursorPos, posData, normalData, true);
    
    ThreeDPoint point;

    // If clicked to the model and not into the space assign the position
    if(posData[3] == 1.f){
        point.pos = glm::vec3(posData[0], posData[1], posData[2]);
        point.normal = glm::vec3(normalData[0], normalData[1], normalData[2]);
    }
    else{
        point.pos = glm::vec3(-1000.f, -1000.f, -1000.f);
        point.normal = glm::vec3(-1000.f, -1000.f, -1000.f);
    }

    //Finish
    delete[] posData;
    delete[] normalData;

    return point;
}

void Mesh::updateObjectIDsTexture(){
    
    glDepthFunc(GL_LESS);
    
    const unsigned int resolution = this->objectIDs.getResolution().x;

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->objectIDs, this->objectIDs.getResolution(), "Mesh::updateObjectIDsTexture");
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {    
        //Use the depth 3D shader
        ShaderSystem::modelObjectID().use();
        ShaderSystem::modelObjectID().setMat4("view", getScene()->camera.viewMatrix);
        ShaderSystem::modelObjectID().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::modelObjectID().setMat4("modelMatrix",getScene()->transformMatrix);

        ShaderSystem::modelObjectID().setInt("objIndicesArraySize", this->objects.size());
        
        for (size_t objI = 0; objI < this->objects.size(); objI++)
        {
            ShaderSystem::modelObjectID().setVec2("objIndices[" + std::to_string(objI) + "]", this->objects[objI].vertIndices / glm::ivec2(3));
        }
        
        this->Draw("Mesh::updateObjectIDsTexture");
    }
    
    //!Finished

    //Set back to default shader
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);

    glDepthFunc(GL_LEQUAL);
}

void Mesh::updateModelPrimitivesTexture(){
    // Generate & bind the framebuffer object to render the model primitives into the modelPrimitives texture
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->face_selection_data.modelPrimitives, this->face_selection_data.modelPrimitives.getResolution(), "Mesh::updateModelPrimitivesTexture");

    glClearColor(0.,0.,0.,0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Seting the primitive ID rendering shader 
    ShaderSystem::primitiveIDShader().use();
    ShaderSystem::primitiveIDShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::primitiveIDShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::primitiveIDShader().setMat4("modelMatrix", getScene()->transformMatrix);
    
    glDepthFunc(GL_LESS);

    ShaderSystem::primitiveIDShader().setInt("emptyFlag", false);
    this->Draw("Mesh::updateModelPrimitivesTexture");

    //Set back to default shader
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);

    glDepthFunc(GL_LEQUAL);
}