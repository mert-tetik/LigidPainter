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

//forward declerations for the utility functions
static void twoDPaintShaderSetBrushProperties(
                                    BrushProperties brushProperties
                                );

static void sendPainterDataToThe3DModelShaderProgram(   
                                                        int selectedColorIndex, Color color1, Color color2, Color color3, float opacity, 
                                                        int selectedPaintingModeIndex, bool usePaintingOver, bool paintingOverGrayScale, 
                                                        bool paintingOverWraping
                                                     );

void Painter::doPaint(glm::mat4 windowOrtho, std::vector<glm::vec2> strokeLocations, int paintingMode, Panel twoDPaintingPanel, Box twoDPaintingBox){

    glm::vec2 firstCursorPos = *Mouse::cursorPos();
    
    //First frame the painting is started
    if(*Mouse::LClick()){
        startCursorPos = *Mouse::cursorPos();
        lastCursorPos = *Mouse::cursorPos();
        frameCounter = 0;
    }

    glm::ivec2 paintingRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier);

    //Bind the painting texture to the painting framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);

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
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture, 0);
    
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
    twoDPaintShaderSetBrushProperties(this->brushProperties);

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(this->brushProperties.spacing);
    if(strokeLocations.size())
        holdLocations = strokeLocations;
    
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
                                                this->threeDimensionalMode, windowOrtho, this->selectedMeshIndex, twoDPaintingBox, this->faceSelection.activated, this->faceSelection.selectedFaces
                                            );

    // If painting mode is set to 3 generate the normal map using paintingTexture8 and write to the paintingTexture16f
    if(paintingMode == 3){
        projectedPaintingTexture.applyNormalMap(8.f, false, true);
    }
    
    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind(); 

    // Send the painter data to the the PBR shader for the painting displaying 
    sendPainterDataToThe3DModelShaderProgram(this->selectedColorIndex, this->color1, this->color2, this->color3, this->brushProperties.opacity, paintingMode, this->usePaintingOver, this->paintingOverGrayScale, this->paintingOverWraping);


}






















// ------------------------------ UTIL FUNCTIONS -----------------------------


static void twoDPaintShaderSetBrushProperties (
                                    BrushProperties brushProperties
                               )
{
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

    // Update the resolution & the format of the projected painting texture in the first frame the painting started
    if(*Mouse::LClick()){
        unsigned int format = 0;
        unsigned int internalFormat = 0;
        if(selectedPaintingModeIndex == 2){
            format = GL_RGBA;
            internalFormat = GL_RGBA16F;
        }
        else if(selectedPaintingModeIndex == 3){
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }
        else{
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        projectedPaintingTexture.update(nullptr, textureRes.x, textureRes.y, GL_LINEAR, format, internalFormat);
    }

    // Generate and bind the capturing framebuffer
    // TODO Don't create the render buffer there
    Framebuffer captureFBO = Framebuffer(projectedPaintingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, textureRes), "Painter::projectThePaintingTexture");
    captureFBO.bind();
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,textureRes.x,textureRes.y);

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
    glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture);
    
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
            ShaderSystem::projectingPaintedTextureShader().setInt("primitiveCount", getModel()->meshes[selectedMeshIndex].indices.size() / 3);
            getModel()->meshes[selectedMeshIndex].Draw(false);
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

    // Finish
    captureFBO.deleteBuffers(false, true);
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
                                                        Texture selectedPrimitives
                                                    )
{
    glDisable(GL_BLEND);

    std::vector<MirrorSide> mirrorSides;
    
    if(oSide.active)
        mirrorSides.push_back(oSide); // 0
    if(oXSide.active)
        mirrorSides.push_back(oXSide); // 1
    if(oYSide.active)
        mirrorSides.push_back(oYSide); // 2
    if(oXYSide.active)
        mirrorSides.push_back(oXYSide); // 3
    if(oZSide.active)
        mirrorSides.push_back(oZSide); // 4
    if(oXZSide.active)
        mirrorSides.push_back(oXZSide); // 5
    if(oYZSide.active)
        mirrorSides.push_back(oYZSide); // 6
    if(oXYZSide.active)
        mirrorSides.push_back(oXYZSide); // 7

    for (size_t i = 0; i < mirrorSides.size(); i++)
    {
        paintingTxtrObj.duplicateTexture(mirrorSides[i].mirroredPaintingTexture);

        bool horizontal = false;
        bool vertical = false;

        glm::vec3 cam = glm::abs(glm::normalize(getScene()->camera.cameraPos));

        //In the X axis
        if(cam.x > 0.5){
            float invertVal = 1.f;
            if(
                mirrorSides[i].effectAxis == oXSide.effectAxis || 
                mirrorSides[i].effectAxis == oXYSide.effectAxis || 
                mirrorSides[i].effectAxis == oXZSide.effectAxis || 
                mirrorSides[i].effectAxis == oXYZSide.effectAxis
            )
                invertVal = -1.f;
            
            horizontal = std::max(mirrorSides[i].effectAxis.z * invertVal, 0.f);
            vertical = std::max(mirrorSides[i].effectAxis.y, 0.f);
        }
        
        
        //In the Z axis
        else{
            float invertVal = 1.f;
            if(
                mirrorSides[i].effectAxis == oZSide.effectAxis || 
                mirrorSides[i].effectAxis == oXZSide.effectAxis || 
                mirrorSides[i].effectAxis == oYZSide.effectAxis || 
                mirrorSides[i].effectAxis == oXYZSide.effectAxis
            )
                invertVal = -1.f;
            
            horizontal = std::max(mirrorSides[i].effectAxis.x * invertVal, 0.f);
            vertical = std::max(mirrorSides[i].effectAxis.y, 0.f);
        }

        if(horizontal || vertical)
            mirrorSides[i].mirroredPaintingTexture.flipTexture(horizontal, vertical);

        glm::vec3 oldCamPos = getScene()->camera.cameraPos;
        glm::vec3 oldCamOrigin = getScene()->camera.originPos;

        {
            Framebuffer FBO = Framebuffer(this->meshPosTxtr, GL_TEXTURE_2D, "Mesh pos normal txtr check for painting");
            FBO.bind();
            
            float* posData = new float[4]; 
            
            glReadPixels(
                            Mouse::cursorPos()->x, 
                            getContext()->windowScale.y - Mouse::cursorPos()->y, 
                            1, 
                            1,
                            GL_RGBA,
                            GL_FLOAT,
                            posData
                        );

            FBO.setColorBuffer(this->meshNormalTxtr, GL_TEXTURE_2D);

            float* normalData = new float[4]; 
            
            glReadPixels(
                            Mouse::cursorPos()->x, 
                            getContext()->windowScale.y - Mouse::cursorPos()->y, 
                            1, 
                            1,
                            GL_RGBA,
                            GL_FLOAT,
                            normalData
                        );

            normalData[0] = normalData[0] * 2.f - 1.f;
            normalData[1] = normalData[1] * 2.f - 1.f;
            normalData[2] = normalData[2] * 2.f - 1.f;
            
            posData[0] = posData[0] * 2.f - 1.f;
            posData[1] = posData[1] * 2.f - 1.f;
            posData[2] = posData[2] * 2.f - 1.f;

            getScene()->camera.cameraPos = glm::vec3(
                                                        posData[0] + normalData[0] * getScene()->camera.radius, 
                                                        posData[1] + normalData[1] * getScene()->camera.radius, 
                                                        posData[2] + normalData[2] * getScene()->camera.radius
                                                    );
        }

        projectThePaintingTexture(selectedTexture, mirrorSides[i].projectedPaintingTexture, mirrorSides[i].mirroredPaintingTexture.ID, mirrorSides[i].depthTexture.ID, 
                                        selectedPaintingModeIndex, brushPropertiesOpacity, threeDimensionalMode, windowOrtho, 
                                        selectedMeshIndex, twoDPaintingBox, mirrorSides[i].getViewMat(glm::vec3(mirrorXOffset,mirrorYOffset,mirrorZOffset)),
                                        faceSelectionActive, selectedPrimitives
                                );

        getScene()->camera.cameraPos = oldCamPos;
        getScene()->camera.originPos = oldCamOrigin;
    }

    getBox()->bindBuffers();

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

    // Update the resolution & the format of the projected painting texture in the first frame the painting started
    if(*Mouse::LClick()){
        unsigned int format = 0;
        unsigned int internalFormat = 0;
        if(selectedPaintingModeIndex == 2){
            format = GL_RGBA;
            internalFormat = GL_RGBA16F;
        }
        else if(selectedPaintingModeIndex == 3){
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }
        else{
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        projectedPaintingTexture.update(nullptr, textureRes.x, textureRes.y, GL_LINEAR, format, internalFormat);
    }

    if(selectedPaintingModeIndex != 6){

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
    else{
        char whitePx[4] = {
                            this->getSelectedColor().getRGB_normalized().r * 127,
                            this->getSelectedColor().getRGB_normalized().g * 127,
                            this->getSelectedColor().getRGB_normalized().b * 127,
                            this->brushProperties.opacity * 127
                        };

        projectedPaintingTexture.update(whitePx, 1, 1, GL_NEAREST);
    }

    glEnable(GL_BLEND);
}