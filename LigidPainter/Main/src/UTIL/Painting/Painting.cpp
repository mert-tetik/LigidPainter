/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Painting to the screen

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

//forward declerations for the utility functions
static void setBrushProperties (Shader paintingShader,BrushProperties brushProperties);
static void setShaderUniforms(Shader paintingShader, glm::mat4 &projection, glm::vec2 videoScale, int frameCounter, Mouse mouse);
static void set3DShaderSideUniforms(Shader tdModelShader,int selectedColorIndex,Color color1,Color color2,Color color3,float opacity ,int selectedPaintingModeIndex);



void Painter::doPaint(Mouse mouse, glm::mat4 windowOrtho, std::vector<Texture> textures){

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
    paintingShader.use();

    //Set uniforms of the painting shader (scale, pos, projection, videoScale, mouseOffset, frame)
    setShaderUniforms(paintingShader,windowOrtho,videoScale,frameCounter,mouse);

    //Set brush properties
    setBrushProperties(paintingShader, this->brushProperties);

    //3D Model shader side of the painting  (set the painting mode, colors, and painting opacity)        
    tdModelShader.use();
    set3DShaderSideUniforms(tdModelShader,selectedColorIndex,color1,color2,color3,this->brushProperties.opacity,selectedPaintingModeIndex);
    
    //Back to 2D painting
    paintingShader.use();

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(mouse,this->brushProperties.spacing);
    paintingShader.setInt("posCount",holdLocations.size());
    for (int i = 0; i < holdLocations.size(); i++)
    {
        std::string target = "positions[" + std::to_string(i) + "]";
        paintingShader.setVec2(target,holdLocations[i]);
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
    
    buttonShader.use();
    
    refreshable = true;
    
    frameCounter++;
}




//---------- UTIL FUNCTIONS ---------


static void setBrushProperties (    Shader paintingShader,
                                    BrushProperties brushProperties
                               ){


    paintingShader.setFloat("brush.radius", brushProperties.radius);
    paintingShader.setFloat("brush.hardness", brushProperties.hardness / 10.f);
    paintingShader.setFloat("brush.sizeJitter", 1.f - brushProperties.sizeJitter / 100.f);
    paintingShader.setFloat("brush.scatter", 1.f - brushProperties.scatter / 100.f);
    paintingShader.setFloat("brush.fade", 1.f - brushProperties.fade / 100.f);
    paintingShader.setFloat("brush.rotation", brushProperties.rotation);
    paintingShader.setFloat("brush.rotationJitter", 1.f - brushProperties.rotationJitter / 100.f);
    paintingShader.setFloat("brush.alphaJitter", 1.f - brushProperties.alphaJitter / 100.f);
    paintingShader.setInt("brush.individualTexture", brushProperties.individualTexture);
    paintingShader.setInt("brush.sinWavePattern", brushProperties.sinWavePattern);
    
    //Bind the texture of the brush
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,brushProperties.brushTexture.ID);
    paintingShader.setFloat("brush.txtr", 0);
}

static void setShaderUniforms(Shader paintingShader, glm::mat4 &projection, glm::vec2 videoScale, int frameCounter, Mouse mouse){
    glm::vec2 scale = videoScale / glm::vec2(2);
    glm::vec3 pos = glm::vec3(videoScale / glm::vec2(2),1.f);
    projection = glm::ortho(0.f,videoScale.x,0.f,videoScale.y);
    
    paintingShader.setVec2("scale", scale); //Cover the screen
    paintingShader.setVec3("pos", pos); //Cover the screen
    paintingShader.setMat4("projection", projection); //Cover the screen
    paintingShader.setVec2("videoScale", videoScale); 
    paintingShader.setVec2("mouseOffset", mouse.mouseOffset);
    paintingShader.setInt("frame", frameCounter);
}

static void set3DShaderSideUniforms(Shader tdModelShader,int selectedColorIndex,Color color1,Color color2,Color color3,float opacity ,int selectedPaintingModeIndex){
    tdModelShader.setInt("brushModeState", selectedPaintingModeIndex);
    if(selectedColorIndex == 0)
        tdModelShader.setVec3("paintingColor", color1.RGB_normalized());
    if(selectedColorIndex == 1)
        tdModelShader.setVec3("paintingColor", color2.RGB_normalized());
    if(selectedColorIndex == 2)
        tdModelShader.setVec3("paintingColor", color3.RGB_normalized());
    
    tdModelShader.setFloat("paintingOpacity", opacity / 100);
}