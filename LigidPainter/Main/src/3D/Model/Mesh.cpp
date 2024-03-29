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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"

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

    Framebuffer FBO = Framebuffer(this->displayingTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)), "Generating mesh displaying texture");

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
    const int resolution = 2024;
    
    this->uvMask = Texture(nullptr, resolution, resolution, GL_LINEAR);

    Framebuffer FBO = Framebuffer(this->uvMask, GL_TEXTURE_2D, "Generating mesh uv mask");
    FBO.bind();
    glViewport(0, 0, resolution, resolution);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::uvMaskShader().use();
    ShaderSystem::uvMaskShader().setVec4("color", glm::vec4(1.));
    ShaderSystem::uvMaskShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f));
    ShaderSystem::uvMaskShader().setFloat("depthToleranceValue", 0);

    this->Draw(false);

    Settings::defaultFramebuffer()->FBO.bind();
    FBO.deleteBuffers(false, false);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName, bool initTxtrs)
{
    if(initTxtrs){
        this->albedo = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->roughness = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->metallic = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->normalMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->heightMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->ambientOcclusion = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->face_selection_data.meshMask = Texture(nullptr, 1024, 1024);
    }

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

    LigidGL::cleanGLErrors();

    Debugger::block("Mesh::Draw start"); // Start
    Debugger::block("Mesh::Draw start"); // End

    Debugger::block("Mesh::Draw"); // End

    // draw mesh
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("Mesh::Draw : Binding VBO");

    glBindVertexArray(VAO);
    LigidGL::testGLError("Mesh::Draw : Binding VAO");

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    LigidGL::testGLError("Mesh::Draw : Drawing Elements");

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
        
        LigidGL::testGLError("Mesh::Draw : Rendering wireframe");
    }

    LigidGL::cleanGLErrors();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : Binding 0 array buffer");
    
    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::Draw : Binding 0 vertex buffer");
    
    Debugger::block("Mesh::Draw"); // End
    
    // always good practice to set everything back to defaults once configured.
}

void Mesh::processHeightMap(){

    std::vector<Vertex> newVertArray = this->vertices;

    char* txtr = new char[this->heightMap.getResolution().x * this->heightMap.getResolution().y * 4];
    this->heightMap.getData(txtr);
    if(getScene()->useHeightMap){
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
                
                newVertArray[i].Position += glm::vec3(((float)red / 127.f - 0.5) * getScene()->heightMapStrength) * newVertArray[i].Normal;
            }
        }
    }

    LigidGL::cleanGLErrors();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("Mesh::processHeightMap : Binding VBO");

    glBindVertexArray(VAO);
    LigidGL::testGLError("Mesh::processHeightMap : Binding VAO");
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, newVertArray.size() * sizeof(Vertex), &newVertArray[0], GL_STATIC_DRAW);  
    LigidGL::testGLError("Mesh::processHeightMap : Buffer data");

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

    LigidGL::cleanGLErrors();
    
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    LigidGL::testGLError("Mesh::setupMesh : Generating VAO");
    glGenBuffers(1, &VBO);
    LigidGL::testGLError("Mesh::setupMesh : Generating VBO");
    glGenBuffers(1, &EBO);
    LigidGL::testGLError("Mesh::setupMesh : Generating EBO");

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("Mesh::setupMesh : Binding VBO");
    glBindVertexArray(VAO);
    LigidGL::testGLError("Mesh::setupMesh : Binding VAO");
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
    LigidGL::testGLError("Mesh::setupMesh : GL_ARRAY_BUFFER data");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    LigidGL::testGLError("Mesh::setupMesh : Binding EBO");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    LigidGL::testGLError("Mesh::setupMesh : GL_ELEMENT_ARRAY_BUFFER data");

    // set the vertex attribute pointers
    
    // vertex Positions
    glEnableVertexAttribArray(0);	
    LigidGL::testGLError("Mesh::setupMesh : glEnableVertexAttribArray 0");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    LigidGL::testGLError("Mesh::setupMesh : glVertexAttribPointer 0");
    
    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    LigidGL::testGLError("Mesh::setupMesh : glEnableVertexAttribArray 1");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    LigidGL::testGLError("Mesh::setupMesh : glVertexAttribPointer 1");
    
    // vertex normals
    glEnableVertexAttribArray(2);	
    LigidGL::testGLError("Mesh::setupMesh : glEnableVertexAttribArray 2");
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    LigidGL::testGLError("Mesh::setupMesh : glVertexAttribPointer 2");
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    LigidGL::testGLError("Mesh::setupMesh : glEnableVertexAttribArray 3");
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    LigidGL::testGLError("Mesh::setupMesh : glVertexAttribPointer 3");
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    LigidGL::testGLError("Mesh::setupMesh : glEnableVertexAttribArray 4");
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    LigidGL::testGLError("Mesh::setupMesh : glVertexAttribPointer 4");
}

void Mesh::getPosNormalOverPoint(glm::vec2 pointPos, float*& posData, float*& normalData, bool readNormal){
        
    const unsigned int resolution = this->getBufferResolutions(1); 
    glm::ivec2 res = glm::ivec2(resolution);
    res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

    pointPos.x /= (Settings::videoScale()->x / res.x); 
    pointPos.y /= (Settings::videoScale()->y / res.y); 
    
    paintingFBO.setColorBuffer(this->meshPosTxtr, GL_TEXTURE_2D);
    paintingFBO.bind();
    
    glReadPixels(
                    pointPos.x, 
                    pointPos.y, 
                    1, 
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    posData
                );

    paintingFBO.setColorBuffer(this->meshNormalTxtr, GL_TEXTURE_2D);

    if(readNormal){
        glReadPixels(
                        pointPos.x, 
                        pointPos.y, 
                        1, 
                        1,
                        GL_RGBA,
                        GL_FLOAT,
                        normalData
                    );
    }
    
    normalData[0] = normalData[0] * 2.f - 1.f;
    normalData[1] = normalData[1] * 2.f - 1.f;
    normalData[2] = normalData[2] * 2.f - 1.f;
    
    posData[0] = posData[0] * 2.f - 1.f;
    posData[1] = posData[1] * 2.f - 1.f;
    posData[2] = posData[2] * 2.f - 1.f;
}

void Mesh::updatePosNormalTexture(){
    
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    unsigned int resolution = this->getBufferResolutions(1); 

    glm::ivec2 res = glm::ivec2(resolution);
    res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

    //Bind the capture framebuffer
    paintingFBO.setRenderbuffer(depthRBO1024);
    paintingFBO.bind();

    if(!this->meshPosTxtr.ID)
        this->meshPosTxtr = Texture(nullptr, res.x, res.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    if(!this->meshNormalTxtr.ID)
        this->meshNormalTxtr = Texture(nullptr, res.x, res.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);

    for (size_t i = 0; i < 2; i++)
    {
        Texture txtr = this->meshPosTxtr;
        if(i == 1)
            txtr = this->meshNormalTxtr;

        //Bind the texture (Painter class public member variable)
        paintingFBO.setColorBuffer(txtr, GL_TEXTURE_2D);

        //Clear the texture
        glViewport(0, 0, getContext()->windowScale.x / (Settings::videoScale()->x / res.x), getContext()->windowScale.y / (Settings::videoScale()->y / res.y));
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use the depth 3D shader
        ShaderSystem::renderModelData().use();
        ShaderSystem::renderModelData().setMat4("view", getScene()->camera.viewMatrix);
        ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
        ShaderSystem::renderModelData().setInt("state", i + 1);

        ShaderSystem::renderModelData().setInt("usingMeshSelection", this->face_selection_data.activated);
        ShaderSystem::renderModelData().setInt("hideUnselected", this->face_selection_data.hideUnselected);
        ShaderSystem::renderModelData().setInt("selectedPrimitiveIDS", 0);
        ShaderSystem::renderModelData().setInt("meshMask", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->face_selection_data.selectedFaces.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->face_selection_data.meshMask.ID);

        //Draw the selected mesh in 3D
        ShaderSystem::renderModelData().setInt("primitiveCount", this->indices.size() / 3);
        this->Draw(false);
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    this->paintingFBO.removeRenderbuffer();

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