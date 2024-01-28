/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Painting to the screen

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

glm::ivec2 Painter::getResolution(){
    // The resolution of the selected texture (painted texture)
    glm::vec2 textureRes = selectedTexture.getResolution();

    if(this->materialPainting && this->selectedMeshIndex < getModel()->meshes.size()){
        if(getModel()->meshes[this->selectedMeshIndex].albedo.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].albedo.getResolution();

        else if(getModel()->meshes[this->selectedMeshIndex].roughness.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].roughness.getResolution();

        else if(getModel()->meshes[this->selectedMeshIndex].metallic.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].metallic.getResolution();

        else if(getModel()->meshes[this->selectedMeshIndex].normalMap.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].normalMap.getResolution();

        else if(getModel()->meshes[this->selectedMeshIndex].heightMap.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].heightMap.getResolution();

        else if(getModel()->meshes[this->selectedMeshIndex].ambientOcclusion.ID)
            textureRes = getModel()->meshes[this->selectedMeshIndex].ambientOcclusion.getResolution();
    
        else
            textureRes = glm::vec2(1024);
    }

    return textureRes;
}

unsigned int Painter::getBufferResolutions(int bufferIndex){
    // 2D Painting texture
    if(bufferIndex == 0)
        return 1024;
    
    // Depth texture res
    else if(bufferIndex == 1)
        return 1024;

    // Low resolution projected painting texture
    else if(bufferIndex == 2)
        return 256;
}

//forward declerations for the utility functions
static void twoDPaintShaderSetBrushProperties(
                                    BrushProperties brushProperties,
                                    bool wrapMode
                                );

static void sendPainterDataToThe3DModelShaderProgram(   
                                                        int selectedColorIndex, Color color1, Color color2, Color color3, float opacity, 
                                                        int selectedPaintingModeIndex, bool usePaintingOver, bool paintingOverGrayScale, 
                                                        bool paintingOverWraping
                                                     );

static glm::vec2 findPos(glm::ivec2 res, float* posData, float* pxs){
    for (size_t y = 0; y < res.y; y++)
    {
        for (size_t x = 0; x < res.x; x++){
            int index = (y * res.x + x) * 4;

            if(true){
                float tolerance = 0.005f;

                if(index + 3 >= res.x * res.y * 4)
                    return glm::vec2(-1.f);

                if(pxs[index + 3] != 0.f){
                    if(pxs[index] >= posData[0] - tolerance && pxs[index] <= posData[0] + tolerance){
                        if(pxs[index + 1] >= posData[1] - tolerance && pxs[index + 1] <= posData[1] + tolerance){
                            if(pxs[index + 2] >= posData[2] - tolerance && pxs[index + 2] <= posData[2] + tolerance){
                                
                                glm::vec2 crsPos = glm::vec2((float)Settings::videoScale()->x / ((float)res.x / (float)x), (float)Settings::videoScale()->y / ((float)res.y / (float)y));
                                
                                return crsPos;
                            }
                        }
                    }
                }
            }
        }
    }
    return glm::vec2(-1.f);
}

Texture posTxtr;

static glm::vec4 lastPos;

void Painter::doPaint(glm::mat4 windowOrtho, std::vector<glm::vec2> strokeLocations, int paintingMode, Panel twoDPaintingPanel, Box twoDPaintingBox, bool highResMode){
    
    glm::vec2 firstCursorPos = *Mouse::cursorPos();
    
    //First frame the painting is started
    if(*Mouse::LClick()){
        startCursorPos = *Mouse::cursorPos();
        lastCursorPos = *Mouse::cursorPos();
        frameCounter = 0;
    }

    glm::ivec2 paintingRes = glm::ivec2(getBufferResolutions(0));

    //Bind the painting texture to the painting framebuffer
    this->paintingFBO.bind();

    if(paintingMode == 2){//If smearing 
        //(Use the 16-bit floating-point RGBA color format)
        paintingTexture = paintingTexture16f; //Bind the 16-bit floating-point RGBA color format to the framebuffer
    }
    else{
        //(Use the 8-bit RGBA color format)
        paintingTexture = paintingTexture8;   //Bind the 8-bit RGBA color format to the framebuffer
    }

    Texture paintingTxtrObj = paintingTexture;
    paintingTxtrObj.duplicateTexture(this->paintingBGTexture);
    
    //Bind the painting texture to the painting framebuffer
    this->paintingFBO.bind();
    this->paintingFBO.setColorBuffer(paintingTexture, GL_TEXTURE_2D);
    
    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0, 0, paintingRes.x, paintingRes.y);
    
    glm::vec2 scale = glm::vec2(0.5f);
    glm::vec3 pos = glm::vec3(0.5f, 0.5f, 1.f);
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);
    
    //Prepeare the 2D painting shader
    ShaderSystem::twoDPainting().use();
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen
    ShaderSystem::twoDPainting().setVec2("paintingRes", paintingRes); 
    ShaderSystem::twoDPainting().setVec2("videoScale", getContext()->windowScale); 
    ShaderSystem::twoDPainting().setInt("frame", frameCounter);
    ShaderSystem::twoDPainting().setInt("bgTxtr", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, paintingBGTexture.ID);
    ShaderSystem::twoDPainting().setInt("redChannelOnly", !(paintingMode == 2));
    ShaderSystem::twoDPainting().setVec3("rgbClr", glm::vec3(0.));

    //Set brush properties
    twoDPaintShaderSetBrushProperties(this->brushProperties, this->wrapMode);
    
    glm::vec3 oldCamPos = getScene()->camera.cameraPos;
    glm::vec3 oldCamOrigin = getScene()->camera.originPos;

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(this->brushProperties.spacing, *Mouse::cursorPos(), lastCursorPos);
    if(strokeLocations.size()){
        holdLocations = strokeLocations;
    }
    else if(this->wrapMode){
        
        holdLocations.clear();
        
        float* posData = new float[4]; 
        float* normalData = new float[4]; 
        
        this->getPosNormalValOverPoint(
                                glm::vec2(
                                            Mouse::cursorPos()->x, 
                                            getContext()->windowScale.y - Mouse::cursorPos()->y
                                        ),
                                posData,
                                normalData
                            );

        getScene()->camera.cameraPos = glm::vec3(
                                                    posData[0] + normalData[0] * 5.1f, 
                                                    posData[1] + normalData[1] * 5.f, 
                                                    posData[2] + normalData[2] * 5.f
                                                );

        getScene()->camera.originPos = glm::vec3(
                                                    posData[0], 
                                                    posData[1], 
                                                    posData[2]
                                                );
        

        posData[0] = (posData[0] + 1.f) / 2.f;
        posData[1] = (posData[1] + 1.f) / 2.f;
        posData[2] = (posData[2] + 1.f) / 2.f;

        const unsigned int resolution = 512; 

        glm::ivec2 res = glm::ivec2(resolution);
        res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

        if(!posTxtr.ID)
            posTxtr = Texture(nullptr, resolution, resolution);

        paintingFBO.setRenderbuffer(depthRBO512);
        paintingFBO.setColorBuffer(posTxtr, GL_TEXTURE_2D);
        paintingFBO.bind();

        //Clear the depth texture
        glViewport(0, 0, (float)getContext()->windowScale.x / ((float)Settings::videoScale()->x / (float)res.x), (float)getContext()->windowScale.y / ((float)Settings::videoScale()->y / (float)res.y));
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 viewMat = getScene()->calculateViewMatrix(getScene()->camera.cameraPos, getScene()->camera.originPos);

        //Use the depth 3D shader
        ShaderSystem::renderModelData().use();
        ShaderSystem::renderModelData().setMat4("view", viewMat);
        ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
        ShaderSystem::renderModelData().setInt("state", 1);

        //Draw the selected mesh in 3D
        if(selectedMeshIndex < getModel()->meshes.size()){
            getModel()->meshes[selectedMeshIndex].Draw(false);
        }

        float* pxs = new float[res.x * res.y * 4]; 
        
        glReadPixels(
                        0, 
                        0, 
                        res.x, 
                        res.y,
                        GL_RGBA,
                        GL_FLOAT,
                        pxs
                    );


        float* lastPosP = new float[4]; 
        lastPosP[0] = lastPos.x;
        lastPosP[1] = lastPos.y;
        lastPosP[2] = lastPos.z;
        lastPosP[3] = lastPos.w;

        glm::vec2 crsPos = findPos(res, posData, pxs);
        glm::vec2 lastCrsPos = findPos(res, lastPosP, pxs);
        
        if(crsPos != glm::vec2(-1.f) && lastCrsPos != glm::vec2(-1.f)){
            glm::vec2 lastDest;
            lastDest.x = crsPos.x - (crsPos.x - lastCrsPos.x);
            lastDest.y = crsPos.y + (crsPos.y - lastCrsPos.y);
            holdLocations = getCursorSubstitution(this->brushProperties.spacing, crsPos, lastDest);
        }
        else if(crsPos != glm::vec2(-1.f))
            holdLocations.push_back(crsPos);

        //std::cout << posData[0] << ", " << posData[1] << ", " << posData[2] << ", " << posData[3] << std::endl;
        //std::cout << lastPosP[0] << ", " << lastPosP[1] << ", " << lastPosP[2] << ", " << lastPosP[3] << std::endl;

        lastPos.x = posData[0];
        lastPos.y = posData[1];
        lastPos.z = posData[2];
        lastPos.w = posData[3];

        delete[] posData;
        delete[] lastPosP;
        delete[] normalData;
        delete[] pxs;

        getBox()->bindBuffers();
        ShaderSystem::twoDPainting().use();
        this->paintingFBO.bind();
        this->paintingFBO.setColorBuffer(paintingTexture, GL_TEXTURE_2D);
        this->paintingFBO.removeRenderbuffer();
        glViewport(0, 0, paintingRes.x, paintingRes.y);
    }
    
    // Set the mouseOffset value
    if(holdLocations.size())
        ShaderSystem::twoDPainting().setVec2("mouseOffset", holdLocations[0] - holdLocations[holdLocations.size()-1]);
    
    // Set the stroke positions
    ShaderSystem::twoDPainting().setInt("posCount",holdLocations.size());
    for (int i = 0; i < holdLocations.size(); i++)
    {
        std::string target = "positions[" + std::to_string(i) + "]";
        ShaderSystem::twoDPainting().setVec2(target,holdLocations[i]);
    }
    
    //Prepeare painting
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    //Painting
    if(!strokeLocations.size()){
        if(glm::distance(startCursorPos,*Mouse::cursorPos()) > this->brushProperties.spacing || *Mouse::LClick()){ //Provide spacing
            startCursorPos = *Mouse::cursorPos();            
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::doPaint : Rendering 2D painted with distance");
        }
    }
    else
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::doPaint : Rendering 2D painted");
    
    //Finish
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);

    lastCursorPos = firstCursorPos;
    this->refreshable = true;
    frameCounter++;

    generateMirroredProjectedPaintingTexture(
                                                this->oSide, this->oXSide, this->oYSide, this->oXYSide, this->oZSide, this->oXZSide, this->oYZSide, 
                                                this->oXYZSide, this->mirrorXOffset, this->mirrorYOffset, this->mirrorZOffset, paintingTxtrObj, this->selectedTexture, 
                                                this->projectedPaintingTexture, paintingMode, this->brushProperties.opacity, 
                                                this->threeDimensionalMode, windowOrtho, this->selectedMeshIndex, twoDPaintingBox, this->faceSelection.activated, 
                                                this->faceSelection.selectedFaces, highResMode
                                            );

    // If painting mode is set to 3 generate the normal map using paintingTexture8 and write to the paintingTexture16f
    if(paintingMode == 3){
        projectedPaintingTexture.applyNormalMap(8.f, false, true);
    }
    
    if(this->wrapMode){
        this->paintingFBO.bind();
        this->paintingFBO.setColorBuffer(this->paintingTexture16f, GL_TEXTURE_2D);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        this->paintingFBO.setColorBuffer(this->paintingTexture8, GL_TEXTURE_2D);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind(); 

    // Send the painter data to the the PBR shader for the painting displaying 
    sendPainterDataToThe3DModelShaderProgram(this->selectedColorIndex, this->color1, this->color2, this->color3, this->brushProperties.opacity, paintingMode, this->usePaintingOver, this->paintingOverGrayScale, this->paintingOverWraping);

    
    getScene()->camera.cameraPos = oldCamPos;
    getScene()->camera.originPos = oldCamOrigin;
}






















// ------------------------------ UTIL FUNCTIONS -----------------------------


static void twoDPaintShaderSetBrushProperties (
                                    BrushProperties brushProperties,
                                    bool wrapMode
                               )
{
    if(wrapMode)
        ShaderSystem::twoDPainting().setFloat("brush.radius", brushProperties.radius);
    else
        ShaderSystem::twoDPainting().setFloat("brush.radius", brushProperties.radius);
    
    ShaderSystem::twoDPainting().setFloat("brush.hardness", brushProperties.hardness);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - brushProperties.sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - brushProperties.scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", brushProperties.fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", brushProperties.rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - brushProperties.rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - brushProperties.alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", brushProperties.individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", brushProperties.sinWavePattern);
    
    //Bind the texture of the brush
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushProperties.brushTexture.ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 0.f, 0.f, 0.f, 1.f };  // Replace r, g, b, a with the desired color values
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glGenerateMipmap(GL_TEXTURE_2D);
    ShaderSystem::twoDPainting().setFloat("brush.txtr", 0);
}



Texture projectedPaintingTextureDup;

void Painter::projectThePaintingTexture(
                                        // Textures
                                        Texture& selectedTexture, 
                                        Texture& projectedPaintingTexture, 
                                        unsigned int paintingTexture, 
                                        unsigned int depthTexture,
                                        
                                        // Painting data
                                        int selectedPaintingModeIndex, 
                                        float brushPropertiesOpacity, 
                                        bool threeDimensionalMode,
                                        glm::mat4 windowOrtho,
                                        int selectedMeshIndex,
                                        Box twoDPaintingBox,
                                        glm::mat4 viewMat,
                                        bool faceSelectionActive,
                                        Texture selectedPrimitives
                                    )
{
    glm::ivec2 projectedPaintingTextureRes = projectedPaintingTexture.getResolution();


    // Generate and bind the capturing framebuffer
    // TODO Don't create the render buffer there
    paintingFBO.setRenderbuffer(depthRBOcustom);
    paintingFBO.setColorBuffer(projectedPaintingTexture, GL_TEXTURE_2D);
    paintingFBO.bind();
    depthRBOcustom.update(GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, glm::ivec2(projectedPaintingTextureRes));
    
    glClearColor(0, 0, 0, 0);
    
    if(this->wrapMode)
        glClear(GL_DEPTH_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0,0,projectedPaintingTextureRes.x,projectedPaintingTextureRes.y);

    // Bind the related shader program
    ShaderSystem::projectingPaintedTextureShader().use();

    //*Fragment
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingTexture", 6);
    ShaderSystem::projectingPaintedTextureShader().setInt("depthTexture", 7);
    ShaderSystem::projectingPaintedTextureShader().setFloat("paintingOpacity", brushPropertiesOpacity);
    ShaderSystem::projectingPaintedTextureShader().setInt("usingMeshSelection", faceSelectionActive);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPrimitiveIDS", 8);
    ShaderSystem::projectingPaintedTextureShader().setInt("meshMask", 10);
    ShaderSystem::projectingPaintedTextureShader().setInt("selectedPaintingModeIndex", selectedPaintingModeIndex);
    
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverTexture", 9);
    ShaderSystem::projectingPaintedTextureShader().setInt("usePaintingOver", this->usePaintingOver);
    ShaderSystem::projectingPaintedTextureShader().setInt("paintingOverGrayScale", this->paintingOverGrayScale);
    ShaderSystem::projectingPaintedTextureShader().setVec3("paintingColor", this->getSelectedColor().getRGB_normalized());
    
    // Bind the painting texture
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, paintingTexture);
    
    // Bind the depth texture for the depth testing
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, selectedPrimitives.ID);

    // Bind the painting over texture
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture.ID);
    
    // Bind the mesh mask texture
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);
    
    // Painting a 3D model
    if(threeDimensionalMode){
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 1);
        
        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", viewMat);
        
        //Draw the UV of the selected model
        if(selectedMeshIndex < getModel()->meshes.size()){
            
            if(wrapMode){
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
            }

            ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", getModel()->meshes[selectedMeshIndex].indices.size() / 3);
            getModel()->meshes[selectedMeshIndex].Draw(false);
            
            if(wrapMode){
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glDisable(GL_BLEND);
            }
        }
    }

    // Painting the 2D scene 
    else{
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 0);

        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", windowOrtho);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        twoDPaintingBox.bindBuffers();
        
        ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", 2);
        LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Painting : Projecting painted texture (For 2D Scene)");
    }

    this->paintingFBO.removeRenderbuffer();
}

static void sendPainterDataToThe3DModelShaderProgram(
                                                        int selectedColorIndex, 
                                                        Color color1, 
                                                        Color color2, 
                                                        Color color3, 
                                                        float opacity, // Brush opacity
                                                        int selectedPaintingModeIndex, 
                                                        bool usePaintingOver, 
                                                        bool paintingOverGrayScale, 
                                                        bool paintingOverWraping
                                                    )
{
    ShaderSystem::tdModelShader().use();
    ShaderSystem::tdModelShader().setInt("brushModeState", selectedPaintingModeIndex);
    ShaderSystem::tdModelShader().setInt("usePaintingOver", usePaintingOver);
}


void Painter::generateMirroredProjectedPaintingTexture(
                                                        // Mirror sides
                                                        MirrorSide& oSide,
                                                        MirrorSide& oXSide,
                                                        MirrorSide& oYSide,
                                                        MirrorSide& oXYSide,
                                                        MirrorSide& oZSide,
                                                        MirrorSide& oXZSide,
                                                        MirrorSide& oYZSide,
                                                        MirrorSide& oXYZSide,
                                                        float mirrorXOffset,
                                                        float mirrorYOffset,
                                                        float mirrorZOffset,
                                                        
                                                        Texture paintingTxtrObj,
                                                        Texture& selectedTexture, 
                                                        Texture& projectedPaintingTexture, 
                                                        
                                                        int selectedPaintingModeIndex, 
                                                        float brushPropertiesOpacity, 
                                                        bool threeDimensionalMode, 
                                                        glm::mat4 windowOrtho, 
                                                        int selectedMeshIndex, 
                                                        Box twoDPaintingBox,
                                                        bool faceSelectionActive,
                                                        Texture selectedPrimitives,
                                                        bool highResMode
                                                    )
{
    glDisable(GL_BLEND);
    
    if(selectedPaintingModeIndex != 6){
        if((*Mouse::LClick() && !this->wrapMode) || this->wrapMode){
            this->updateDepthTexture();
        }

        std::vector<MirrorSide*> mirrorSides;
        
        if(oSide.active)
            mirrorSides.push_back(&oSide); // 0
        if(oXSide.active)
            mirrorSides.push_back(&oXSide); // 1
        if(oYSide.active)
            mirrorSides.push_back(&oYSide); // 2
        if(oXYSide.active)
            mirrorSides.push_back(&oXYSide); // 3
        if(oZSide.active)
            mirrorSides.push_back(&oZSide); // 4
        if(oXZSide.active)
            mirrorSides.push_back(&oXZSide); // 5
        if(oYZSide.active)
            mirrorSides.push_back(&oYZSide); // 6
        if(oXYZSide.active)
            mirrorSides.push_back(&oXYZSide); // 7

        bool use16f = false;
        if(selectedPaintingModeIndex == 2){
            use16f = true;
        }

        // Update the projected painting textures
        {
            if(use16f){
                if(highResMode){
                    this->projectedPaintingTexture = this->projectedPaintingTexture16f;
                    if(this->projectedPaintingTexture16f.getResolution() != this->getResolution())
                        this->projectedPaintingTexture16f.update(nullptr, this->getResolution().x, this->getResolution().y, GL_LINEAR, GL_RGBA, GL_RGBA16F);  
                }
                else
                    this->projectedPaintingTexture = this->projectedPaintingTexture16fLow;  

            }
            else{
                if(highResMode){
                    this->projectedPaintingTexture = this->projectedPaintingTexture8;  
                    if(this->projectedPaintingTexture8.getResolution() != this->getResolution())
                        this->projectedPaintingTexture8.update(nullptr, this->getResolution().x, this->getResolution().y, GL_LINEAR);  
                }
                else
                    this->projectedPaintingTexture = this->projectedPaintingTexture8Low;  
            }

            for (size_t i = 0; i < mirrorSides.size(); i++)
            {
                if(use16f){
                    if(highResMode){
                        mirrorSides[i]->projectedPaintingTexture = mirrorSides[i]->projectedPaintingTexture16f;  
                        if(mirrorSides[i]->projectedPaintingTexture16f.getResolution() != this->getResolution())
                            mirrorSides[i]->projectedPaintingTexture16f.update(nullptr, this->getResolution().x, this->getResolution().y, GL_LINEAR, GL_RGBA, GL_RGBA16F);  
                    }
                    else
                        mirrorSides[i]->projectedPaintingTexture = mirrorSides[i]->projectedPaintingTexture16fLow;  

                }
                else{
                    if(highResMode){
                        mirrorSides[i]->projectedPaintingTexture = mirrorSides[i]->projectedPaintingTexture8;  
                        if(mirrorSides[i]->projectedPaintingTexture8.getResolution() != this->getResolution())
                            mirrorSides[i]->projectedPaintingTexture8.update(nullptr, this->getResolution().x, this->getResolution().y, GL_LINEAR);  
                    }
                    else
                        mirrorSides[i]->projectedPaintingTexture = mirrorSides[i]->projectedPaintingTexture8Low;  
                }
            }
        }
        

        for (size_t i = 0; i < mirrorSides.size(); i++)
        {
            paintingTxtrObj.duplicateTexture(mirrorSides[i]->mirroredPaintingTexture);

            bool horizontal = false;
            bool vertical = false;

            glm::vec3 cam = glm::abs(glm::normalize(getScene()->camera.cameraPos));

            //In the X axis
            if(cam.x > 0.5){
                float invertVal = 1.f;
                if(
                    mirrorSides[i]->effectAxis == oXSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oXYSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oXZSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oXYZSide.effectAxis
                )
                    invertVal = -1.f;
                
                horizontal = std::max(mirrorSides[i]->effectAxis.z * invertVal, 0.f);
                vertical = std::max(mirrorSides[i]->effectAxis.y, 0.f);
            }
            
            
            //In the Z axis
            else{
                float invertVal = 1.f;
                if(
                    mirrorSides[i]->effectAxis == oZSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oXZSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oYZSide.effectAxis || 
                    mirrorSides[i]->effectAxis == oXYZSide.effectAxis
                )
                    invertVal = -1.f;
                
                horizontal = std::max(mirrorSides[i]->effectAxis.x * invertVal, 0.f);
                vertical = std::max(mirrorSides[i]->effectAxis.y, 0.f);
            }

            if(horizontal || vertical)
                mirrorSides[i]->mirroredPaintingTexture.flipTexture(horizontal, vertical);

            Texture projectedPaintingtxtr = mirrorSides[i]->projectedPaintingTexture;

            if(mirrorSides.size() == 1){
                projectedPaintingtxtr = this->projectedPaintingTexture;
            }

            projectThePaintingTexture(selectedTexture, projectedPaintingtxtr, mirrorSides[i]->mirroredPaintingTexture.ID, mirrorSides[i]->depthTexture.ID, 
                                            selectedPaintingModeIndex, brushPropertiesOpacity, threeDimensionalMode, windowOrtho, 
                                            selectedMeshIndex, twoDPaintingBox, mirrorSides[i]->getViewMat(glm::vec3(mirrorXOffset,mirrorYOffset,mirrorZOffset)),
                                            faceSelectionActive, selectedPrimitives
                                    );
        }

        getBox()->bindBuffers();
        
        if(mirrorSides.size() != 1){
            // The resolution of the selected texture (painted texture)
            glm::vec2 textureRes = this->getResolution();

            // Generate and bind the capturing framebuffer
            Framebuffer captureFBO = Framebuffer(projectedPaintingTexture, GL_TEXTURE_2D, "Painter::generateMirroredProjectedPaintingTexture");
            captureFBO.bind();
            
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0,0,textureRes.x,textureRes.y);

            ShaderSystem::projectedPaintingTextureMixerShader().use();

            ShaderSystem::projectedPaintingTextureMixerShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
            ShaderSystem::projectedPaintingTextureMixerShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
            ShaderSystem::projectedPaintingTextureMixerShader().setVec2("scale", glm::vec2(0.5f));

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("redChannel", false);
            
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr1", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, oSide.projectedPaintingTexture.ID);
            
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2Active", oXSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr2", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, oXSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3Active", oYSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr3", 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, oYSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4Active", oXYSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr4", 4);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, oXYSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5Active", oZSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr5", 5);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, oZSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6Active", oXZSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr6", 6);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, oXZSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7Active", oYZSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr7", 7);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, oYZSide.projectedPaintingTexture.ID);

            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8Active", oXYZSide.active);
            ShaderSystem::projectedPaintingTextureMixerShader().setInt("txtr8", 8);
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, oXYZSide.projectedPaintingTexture.ID);

            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::generateMirroredProjectedPaintingTexture : Render result");

            captureFBO.deleteBuffers(false, false);
        }
    }
    
    // If bucket painting - Fill the projected painting texture with a color
    else{
        
        char whitePx[4] = {
                            this->getSelectedColor().getRGB_normalized().r * 127,
                            this->getSelectedColor().getRGB_normalized().g * 127,
                            this->getSelectedColor().getRGB_normalized().b * 127,
                            this->brushProperties.opacity * 127
                        };

        projectedPaintingTexture = projectedPaintingTexture8Low; 

        glm::ivec2 res = projectedPaintingTexture.getResolution();
        char* pxs = new char[res.x * res.y * 4];

        for (size_t y = 0; y < res.y; y++)
        {
            for (size_t x = 0; x < res.x; x++){
                int index = (y * res.x + x) * 4;
            
                if(index + 3 < res.x * res.y * 4){
                    pxs[index + 0] = whitePx[0]; 
                    pxs[index + 1] = whitePx[1]; 
                    pxs[index + 2] = whitePx[2]; 
                    pxs[index + 3] = whitePx[3]; 
                }
            }
        }

        projectedPaintingTexture.update(pxs, res.x, res.y, GL_LINEAR);

        delete[] pxs;
    }

    glEnable(GL_BLEND);
}