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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

//forward declerations for the utility functions
static void setBrushProperties (BrushProperties brushProperties);
static void setShaderUniforms(glm::mat4 &projection, glm::vec2 videoScale, int frameCounter, Mouse& mouse);
static void set3DShaderSideUniforms(int selectedColorIndex,Color color1,Color color2,Color color3,float opacity ,int selectedPaintingModeIndex);



void Painter::doPaint(Mouse& mouse, glm::mat4 windowOrtho, std::vector<Texture> textures, Context context){

    glm::vec2 firstCursorPos = mouse.cursorPos;
    
    //First frame the painting is started
    if(mouse.LClick){
        startCursorPos = mouse.cursorPos;
        lastCursorPos = mouse.cursorPos;
        frameCounter = 0;
    }

    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0,0,videoScale.x,videoScale.y);

    //Bind the painting texture to the painting framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);

    if(selectedPaintingModeIndex == 2){//If smearing 
        //(Use the 16-bit floating-point RGBA color format)
        paintingTexture = paintingTexture16f; //Bind the 16-bit floating-point RGBA color format to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture16f, 0);
    }
    else{
        //(Use the 8-bit RGBA color format)
        paintingTexture = paintingTexture8;   //Bind the 8-bit RGBA color format to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
    }
    
    //Prepeare the 2D painting shader
    ShaderSystem::twoDPainting().use();

    //Set uniforms of the painting shader (scale, pos, projection, videoScale, mouseOffset, frame)
    setShaderUniforms(windowOrtho,context.windowScale,frameCounter,mouse);

    //Set brush properties
    setBrushProperties(this->brushProperties);

    //3D Model shader side of the painting  (set the painting mode, colors, and painting opacity)        
    ShaderSystem::tdModelShader().use();
    set3DShaderSideUniforms(selectedColorIndex,color1,color2,color3,this->brushProperties.opacity,selectedPaintingModeIndex);
    
    //Back to 2D painting
    ShaderSystem::twoDPainting().use();

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(mouse,this->brushProperties.spacing);
    ShaderSystem::twoDPainting().setInt("posCount",holdLocations.size());
    for (int i = 0; i < holdLocations.size(); i++)
    {
        std::string target = "positions[" + std::to_string(i) + "]";
        ShaderSystem::twoDPainting().setVec2(target,holdLocations[i]);
    }
    
    //Prepeare painting
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    //Painting
    if(glm::distance(startCursorPos,mouse.cursorPos) > this->brushProperties.spacing){ //Provide spacing
        startCursorPos = mouse.cursorPos;            
        glDrawArrays(GL_TRIANGLES,0,6);
    }
    
    //Finish
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER,0); //Bind the default framebuffer
    
    lastCursorPos = firstCursorPos;
    
    ShaderSystem::buttonShader().use();
    
    refreshable = true;
    
    frameCounter++;
}




//---------- UTIL FUNCTIONS ---------


static void setBrushProperties (
                                    BrushProperties brushProperties
                               )
{
    ShaderSystem::twoDPainting().setFloat("brush.radius", brushProperties.radius);
    ShaderSystem::twoDPainting().setFloat("brush.hardness", brushProperties.hardness / 10.f);
    ShaderSystem::twoDPainting().setFloat("brush.sizeJitter", 1.f - brushProperties.sizeJitter / 100.f);
    ShaderSystem::twoDPainting().setFloat("brush.scatter", 1.f - brushProperties.scatter / 100.f);
    ShaderSystem::twoDPainting().setFloat("brush.fade", 1.f - brushProperties.fade / 100.f);
    ShaderSystem::twoDPainting().setFloat("brush.rotation", brushProperties.rotation);
    ShaderSystem::twoDPainting().setFloat("brush.rotationJitter", 1.f - brushProperties.rotationJitter / 100.f);
    ShaderSystem::twoDPainting().setFloat("brush.alphaJitter", 1.f - brushProperties.alphaJitter / 100.f);
    ShaderSystem::twoDPainting().setInt("brush.individualTexture", brushProperties.individualTexture);
    ShaderSystem::twoDPainting().setInt("brush.sinWavePattern", brushProperties.sinWavePattern);
    
    //Bind the texture of the brush
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushProperties.brushTexture.ID);
    ShaderSystem::twoDPainting().setFloat("brush.txtr", 0);
}

static void setShaderUniforms(glm::mat4 &projection, glm::vec2 videoScale, int frameCounter, Mouse& mouse){
    glm::vec2 scale = videoScale / glm::vec2(2);
    glm::vec3 pos = glm::vec3(videoScale / glm::vec2(2),1.f);
    projection = glm::ortho(0.f,videoScale.x,0.f,videoScale.y);
    
    ShaderSystem::twoDPainting().setVec2("scale", scale); //Cover the screen
    ShaderSystem::twoDPainting().setVec3("pos", pos); //Cover the screen
    ShaderSystem::twoDPainting().setMat4("projection", projection); //Cover the screen
    ShaderSystem::twoDPainting().setVec2("videoScale", videoScale); 
    ShaderSystem::twoDPainting().setVec2("mouseOffset", mouse.mouseOffset);
    ShaderSystem::twoDPainting().setInt("frame", frameCounter);
}

static void set3DShaderSideUniforms(int selectedColorIndex, Color color1, Color color2, Color color3, float opacity, int selectedPaintingModeIndex){
    ShaderSystem::tdModelShader().setInt("brushModeState", selectedPaintingModeIndex);
    if(selectedColorIndex == 0)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color1.getRGB_normalized());
    if(selectedColorIndex == 1)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color2.getRGB_normalized());
    if(selectedColorIndex == 2)
        ShaderSystem::tdModelShader().setVec3("paintingColor", color3.getRGB_normalized());
    
    ShaderSystem::tdModelShader().setFloat("paintingOpacity", opacity / 100);
}