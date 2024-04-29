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

#include "3D/Mesh/Mesh.hpp"

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
        this->displayingTxtr.update((char*)nullptr, displayRes, displayRes, GL_LINEAR);

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
    this->Draw();
    
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

    this->Draw();

    Settings::defaultFramebuffer()->FBO.bind();
    FBO.deleteBuffers(false, false);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName, bool initTxtrs)
{
    if(!vertices.size())
        vertices.push_back(Vertex());

    if(!indices.size())
        indices.push_back(0);
    
    this->vertices = vertices;
    this->indices = indices;
    this->materialName = materialName;

    getContext()->window.makeContextCurrent();
    this->setupMesh(&this->VBO, &this->VAO, &this->EBO);   
    getSecondContext()->window.makeContextCurrent();
    this->setupMesh(&this->VBO_2, &this->VAO_2, &this->EBO_2);   
    getContext()->window.makeContextCurrent();

    if(initTxtrs){
        this->albedo = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->roughness = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->metallic = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->normalMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->heightMap = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->ambientOcclusion = Texture(nullptr, 1024, 1024, GL_LINEAR);
        this->objectIDs = Texture(nullptr, 1024, 1024, GL_LINEAR, GL_RG, GL_RG32F);
        
        this->face_selection_data.modelPrimitives = Texture(nullptr, 1024, 1024, GL_NEAREST, GL_RED, GL_R32F);
        char whitePx[] = {127, 127, 127, 127};
        this->face_selection_data.meshMask = Texture(whitePx, 1, 1, GL_NEAREST);


        this->meshPosTxtr = Texture(nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);
        this->meshNormalTxtr = Texture(nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);

        if(indices.size()){
            this->face_selection_data.meshMask_display_txtr = Texture(nullptr, 512, 512);
            this->face_selection_data.meshMask_display_txtr.proceduralProps.proceduralID = 24;
            this->face_selection_data.meshMask_display_txtr.proceduralProps.proceduralnverted = false;
            this->face_selection_data.meshMask_display_txtr.generateProceduralDisplayingTexture(512, 1);
            
            this->face_selection_data.selectedPrimitiveIDs.resize(this->indices.size() / 3);
            std::fill(this->face_selection_data.selectedPrimitiveIDs.begin(), this->face_selection_data.selectedPrimitiveIDs.end(), 0);

            glGenTextures(1, &this->face_selection_data.selectedFaces.ID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->face_selection_data.selectedFaces.ID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, std::ceil(sqrt(this->face_selection_data.selectedPrimitiveIDs.size())), std::ceil(sqrt(this->face_selection_data.selectedPrimitiveIDs.size())), 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    generateUVMask();

    generateDisplayingTexture();
}

// Render the mesh
void Mesh::Draw() 
{

    LigidGL::cleanGLErrors();

    Debugger::block("Mesh::Draw start"); // Start
    Debugger::block("Mesh::Draw start"); // End

    Debugger::block("Mesh::Draw"); // End

    // draw mesh
    glBindBuffer(GL_ARRAY_BUFFER, (getContext()->window.isContextCurrent()) ? VBO : VBO_2);
    LigidGL::testGLError("Mesh::Draw : Binding VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (getContext()->window.isContextCurrent()) ? EBO : EBO_2);
    LigidGL::testGLError("Mesh::Draw : Binding EBO"); 
    
    glBindVertexArray((getContext()->window.isContextCurrent()) ? VAO : VAO_2);
    LigidGL::testGLError("Mesh::Draw : Binding VAO");
    
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    LigidGL::testGLError("Mesh::Draw : Drawing Elements");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::Draw : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : Binding 0 array buffer");
    
    
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

    glBindBuffer(GL_ARRAY_BUFFER, (getContext()->window.isContextCurrent()) ? VBO : VBO_2);
    LigidGL::testGLError("Mesh::processHeightMap : Binding VBO");

    glBindVertexArray((getContext()->window.isContextCurrent()) ? VAO : VAO_2);
    LigidGL::testGLError("Mesh::processHeightMap : Binding VAO");
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, newVertArray.size() * sizeof(Vertex), &newVertArray[0], GL_STATIC_DRAW);  
    LigidGL::testGLError("Mesh::processHeightMap : Buffer data");

    Settings::defaultFramebuffer()->FBO.bind();

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::processHeightMap : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::processHeightMap : Binding 0 element array buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::processHeightMap : Binding 0 array buffer");

    delete[] txtr;
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh(unsigned int* VBO, unsigned int* VAO, unsigned int* EBO)
{
    LigidGL::cleanGLErrors();
    
    // create buffers/arrays
    glGenVertexArrays(1, VAO);
    LigidGL::testGLError("Mesh::setupMesh : Generating VAO");
    glGenBuffers(1, VBO);
    LigidGL::testGLError("Mesh::setupMesh : Generating VBO");
    glGenBuffers(1, EBO);
    LigidGL::testGLError("Mesh::setupMesh : Generating EBO");

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    LigidGL::testGLError("Mesh::setupMesh : Binding VBO");
    glBindVertexArray(*VAO);
    LigidGL::testGLError("Mesh::setupMesh : Binding VAO");
    // load data into vertex buffers
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
    LigidGL::testGLError("Mesh::setupMesh : GL_ARRAY_BUFFER data");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
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

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::setupMesh : Binding 0 vertex buffer");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::setupMesh : Binding 0 array buffer");
    
}

static Framebuffer getPosNormalOverPoint_FBO;
void Mesh::getPosNormalOverPoint(glm::vec2 pointPos, float*& posData, float*& normalData, bool readNormal){
        
    const unsigned int resolution = this->meshPosTxtr.getResolution().x; 

    pointPos.x = UTIL::new_value_range(pointPos.x, 0, getContext()->windowScale.x, 0, resolution);
    pointPos.y = UTIL::new_value_range(pointPos.y, 0, getContext()->windowScale.y, 0, resolution);

    if(!getPosNormalOverPoint_FBO.ID){
        getPosNormalOverPoint_FBO.generate();
        getPosNormalOverPoint_FBO.purpose = "Mesh::getPosNormalOverPoint getPosNormalOverPoint_FBO";
    }

    getPosNormalOverPoint_FBO.setColorBuffer(this->meshPosTxtr, GL_TEXTURE_2D);
    getPosNormalOverPoint_FBO.bind();
    
    glReadPixels(
                    pointPos.x, 
                    pointPos.y, 
                    1, 
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    posData
                );

    getPosNormalOverPoint_FBO.setColorBuffer(this->meshNormalTxtr, GL_TEXTURE_2D);

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

static Framebuffer updatePosNormalTexture_FBO;
void Mesh::updatePosNormalTexture(){
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    unsigned int resolution = this->meshPosTxtr.getResolution().x; 
    
    if(!updatePosNormalTexture_FBO.ID){
        updatePosNormalTexture_FBO = Framebuffer(this->meshPosTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "updatePosNormalTexture_FBO");
    }
    updatePosNormalTexture_FBO.setColorBuffer(this->meshPosTxtr, GL_TEXTURE_2D);
    
    updatePosNormalTexture_FBO.bind();

    for (size_t i = 0; i < 2; i++)
    {
        Texture txtr = this->meshPosTxtr;
        if(i == 1)
            txtr = this->meshNormalTxtr;

        //Bind the texture (Painter class public member variable)
        updatePosNormalTexture_FBO.setColorBuffer(txtr, GL_TEXTURE_2D);

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

        ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::renderModelData(), *this, GL_TEXTURE0, GL_TEXTURE1);

        //Draw the selected mesh in 3D
        ShaderSystem::renderModelData().setInt("primitiveCount", this->indices.size() / 3);
        this->Draw();
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

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

static Framebuffer updateObjectIDsTexture_FBO;
void Mesh::updateObjectIDsTexture(){
    glDepthFunc(GL_LESS);
    
    const unsigned int resolution = this->objectIDs.getResolution().x;

    if(!updateObjectIDsTexture_FBO.ID)
        updateObjectIDsTexture_FBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "updateObjectIDsTexture");
    
    updateObjectIDsTexture_FBO.setColorBuffer(this->objectIDs, GL_TEXTURE_2D);
    updateObjectIDsTexture_FBO.bind();
    
    //Clear the object ids texture
    glViewport(0, 0, resolution, resolution);
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
        
        this->Draw();
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
}

static Framebuffer updateModelPrimitivesTexture_FBO;
void Mesh::updateModelPrimitivesTexture(){
    // Generate & bind the framebuffer object to render the model primitives into the modelPrimitives texture
    if(!updateModelPrimitivesTexture_FBO.ID)
        updateModelPrimitivesTexture_FBO = Framebuffer(this->face_selection_data.modelPrimitives, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, this->face_selection_data.modelPrimitives.getResolution()), "Face selection");

    updateModelPrimitivesTexture_FBO.setColorBuffer(this->face_selection_data.modelPrimitives, GL_TEXTURE_2D);
    updateModelPrimitivesTexture_FBO.bind();

    glClearColor(0.,0.,0.,0.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, this->face_selection_data.modelPrimitives.getResolution().x, this->face_selection_data.modelPrimitives.getResolution().y);

    // Seting the primitive ID rendering shader 
    ShaderSystem::primitiveIDShader().use();
    ShaderSystem::primitiveIDShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::primitiveIDShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::primitiveIDShader().setMat4("modelMatrix", getScene()->transformMatrix);
    
    glDepthFunc(GL_LESS);

    ShaderSystem::primitiveIDShader().setInt("emptyFlag", false);
    this->Draw();

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
}

void Mesh::update_vertex_buffers(){
    if(!vertices.size())
        vertices.push_back(Vertex());

    if(!indices.size())
        indices.push_back(0);

    LigidGL::cleanGLErrors();
    
    glBindBuffer(GL_ARRAY_BUFFER, (getContext()->window.isContextCurrent()) ? VBO : VBO_2);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding VBO");
    
    glBindVertexArray((getContext()->window.isContextCurrent()) ? VAO : VAO_2);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding VAO");

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);  
    LigidGL::testGLError("Mesh::update_vertex_buffers : GL_ARRAY_BUFFER data");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (getContext()->window.isContextCurrent()) ? EBO : EBO_2);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding EBO");

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);  
    LigidGL::testGLError("Mesh::update_vertex_buffers : GL_ELEMENT_ARRAY_BUFFER data");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 element array buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 array buffer");
}