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
static void setBrushProperties (Shader paintingShader,float radius,float hardness,float opacity,float spacing,float sizeJitter,float scatter,float fade,float rotation,float rotationJitter,float alphaJitter,bool individualTexture,bool sinWavePattern,unsigned int textureID);
static void setShaderUniforms(Shader paintingShader, glm::mat4 &projection, glm::vec2 videoScale, int frameCounter, Mouse mouse);
static void set3DShaderSideUniforms(Shader tdModelShader,int selectedColorIndex,Color color1,Color color2,Color color3,float opacity ,int selectedPaintingModeIndex);



void Painter::doPaint(Mouse mouse,float radius,float hardness,float opacity,float spacing,float sizeJitter,float scatter,float fade,float rotation,
                      float rotationJitter,float alphaJitter,bool individualTexture,bool sinWavePattern,std::vector<Texture> textures){

    glm::vec2 firstCursorPos = mouse.cursorPos;
    
    //First frame the painting is started
    if(mouse.LClick){
        startCursorPos = mouse.cursorPos;
        lastCursorPos = mouse.cursorPos;
        frameCounter = 0;
    }

    //Cover the whole monitor (since we are painting to the screen)
    glViewport(0,0,videoScale.x,videoScale.y);

    //Bind the painting texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);

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
    setShaderUniforms(paintingShader,projection,videoScale,frameCounter,mouse);

    //Set brush properties
    setBrushProperties(paintingShader,radius,hardness,opacity,spacing,sizeJitter,scatter,fade,rotation,rotationJitter,alphaJitter,individualTexture,sinWavePattern,brushTexture.ID);

    //3D Model shader side of the painting  (set the painting mode, colors, and painting opacity)        
    tdModelShader.use();
    set3DShaderSideUniforms(tdModelShader,selectedColorIndex,color1,color2,color3,opacity,selectedPaintingModeIndex);
    
    //Back to 2D painting
    paintingShader.use();

    //Stroke positions
    std::vector<glm::vec2> holdLocations = getCursorSubstitution(mouse,spacing);
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
    if(glm::distance(startCursorPos,mouse.cursorPos) > spacing){ //Provide spacing
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
                                    float radius,
                                    float hardness,
                                    float opacity,
                                    float spacing,
                                    float sizeJitter,
                                    float scatter,
                                    float fade,
                                    float rotation,
                                    float rotationJitter,
                                    float alphaJitter,
                                    bool individualTexture,
                                    bool sinWavePattern,
                                    unsigned int textureID
                               ){

    paintingShader.setFloat("brush.radius", radius);
    paintingShader.setFloat("brush.hardness", hardness);
    paintingShader.setFloat("brush.sizeJitter", sizeJitter);
    paintingShader.setFloat("brush.scatter", scatter);
    paintingShader.setFloat("brush.fade", fade);
    paintingShader.setFloat("brush.rotation", rotation);
    paintingShader.setFloat("brush.rotationJitter", rotationJitter);
    paintingShader.setFloat("brush.alphaJitter", alphaJitter);
    paintingShader.setInt("brush.individualTexture", individualTexture);
    paintingShader.setInt("brush.sinWavePattern", sinWavePattern);
    
    //Bind the texture of the brush
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textureID);
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
        tdModelShader.setVec3("paintingColor", color1.RGB / glm::vec3(255.f));
    if(selectedColorIndex == 1)
        tdModelShader.setVec3("paintingColor", color2.RGB / glm::vec3(255.f));
    if(selectedColorIndex == 2)
        tdModelShader.setVec3("paintingColor", color3.RGB / glm::vec3(255.f));
    tdModelShader.setFloat("paintingOpacity", opacity);
}