/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include "UTIL/Util.hpp"
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
static void setBrushProperties (BrushProperties brushProperties);
static void setShaderUniforms(glm::mat4 &projection, glm::vec2 resolution, glm::vec2 videoScale, int frameCounter);
static void set3DShaderSideUniforms(int selectedColorIndex,Color color1,Color color2,Color color3,float opacity,
                                    int selectedPaintingModeIndex,bool usePaintingOver, bool paintingOverGrayScale, bool paintingOverWraping);



void Painter::doPaint(glm::mat4 windowOrtho, std::vector<glm::vec2> strokeLocations, int paintingMode){

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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture16f, 0);
    }
    else{
        //(Use the 8-bit RGBA color format)
        paintingTexture = paintingTexture8;   //Bind the 8-bit RGBA color format to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
    }

    Texture paintingTxtrObj = paintingTexture;
    paintingTxtrObj.duplicateTexture(this->paintingBGTexture);
    
    //Bind the painting texture to the painting framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);
    
    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0, 0, paintingRes.x, paintingRes.y);
    
    //Prepeare the 2D painting shader
    ShaderSystem::twoDPainting().use();

    glm::mat4 projection = glm::ortho(0.f, (float)paintingRes.x, (float)paintingRes.y, 0.f);

    //Set uniforms of the painting shader (scale, pos, projection, mouseOffset, frame)
    setShaderUniforms(projection, paintingRes, *Settings::videoScale(), frameCounter);
    ShaderSystem::twoDPainting().setInt("bgTxtr", 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paintingTxtrObj.ID);

    //Set brush properties
    setBrushProperties(this->brushProperties);

    //3D Model shader side of the painting  (set the painting mode, colors, and painting opacity)        
    ShaderSystem::tdModelShader().use();
    set3DShaderSideUniforms(selectedColorIndex, color1, color2, color3, this->brushProperties.opacity, paintingMode, this->usePaintingOver, this->paintingOverGrayScale, this->paintingOverWraping);
    
    //Back to 2D painting
    ShaderSystem::twoDPainting().use();

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(this->brushProperties.spacing);
    if(strokeLocations.size())
        holdLocations = strokeLocations;

    if(holdLocations.size())
        ShaderSystem::twoDPainting().setVec2("mouseOffset", holdLocations[0] - holdLocations[holdLocations.size()-1]);
    

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
        if(glm::distance(startCursorPos,*Mouse::cursorPos()) > this->brushProperties.spacing){ //Provide spacing
            startCursorPos = *Mouse::cursorPos();            
            glDrawArrays(GL_TRIANGLES,0,6);
        }
    }
    else
        glDrawArrays(GL_TRIANGLES,0,6);
    
    //Finish
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);

    Settings::defaultFramebuffer()->FBO.bind(); //Bind the default framebuffer
    
    lastCursorPos = firstCursorPos;
    
    ShaderSystem::buttonShader().use();
    
    refreshable = true;
    
    frameCounter++;
    
    if(paintingMode == 3){
        Texture txtrObj = this->paintingTexture;
        txtrObj.generateNormalMap(this->paintingTexture16f, paintingRes, 8.f, false, true);
        this->paintingTexture = this->paintingTexture16f;
    }
}




//---------- UTIL FUNCTIONS ---------


static void setBrushProperties (
                                    BrushProperties brushProperties
                               )
{
    ShaderSystem::twoDPainting().setFloat("brush.radius", brushProperties.radius);
    ShaderSystem::twoDPainting().setFloat("brush.hardness", brushProperties.hardness);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - brushProperties.sizeJitter);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - brushProperties.scatter);
    ShaderSystem::twoDPainting().setFloat("brush.fade", 1.f - brushProperties.fade);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", brushProperties.rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - brushProperties.rotationJitter);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - brushProperties.alphaJitter);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", brushProperties.individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", brushProperties.sinWavePattern);
    
    //Bind the texture of the brush
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushProperties.brushTexture.ID);
    ShaderSystem::twoDPainting().setFloat("brush.txtr", 0);
}

static void setShaderUniforms(glm::mat4 &projection, glm::vec2 resolution, glm::vec2 videoScale, int frameCounter){
    glm::vec2 scale = resolution / glm::vec2(2);
    glm::vec3 pos = glm::vec3(resolution / glm::vec2(2),1.f);
    projection = glm::ortho(0.f,resolution.x,0.f,resolution.y);
    
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen
    ShaderSystem::twoDPainting().setVec2("paintingRes", resolution); 
    ShaderSystem::twoDPainting().setVec2("videoScale", videoScale); 
    ShaderSystem::twoDPainting().setInt("frame", frameCounter);
}

static void set3DShaderSideUniforms(int selectedColorIndex, Color color1, Color color2, Color color3, float opacity, int selectedPaintingModeIndex, bool usePaintingOver, bool paintingOverGrayScale, bool paintingOverWraping){
    ShaderSystem::tdModelShader().setInt("brushModeState", selectedPaintingModeIndex);
    if(selectedColorIndex == 0)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color1.getRGB_normalized());
    if(selectedColorIndex == 1)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color2.getRGB_normalized());
    if(selectedColorIndex == 2)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color3.getRGB_normalized());
    
    ShaderSystem::tdModelShader().setFloat("paintingOpacity", opacity);
    ShaderSystem::tdModelShader().setInt("usePaintingOver", usePaintingOver);
    ShaderSystem::tdModelShader().setInt("paintingOverGrayScale", paintingOverGrayScale);
    ShaderSystem::tdModelShader().setInt("paintingOverWraping", paintingOverWraping);
}