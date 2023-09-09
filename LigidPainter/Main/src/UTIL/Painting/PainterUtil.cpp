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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

#include "tinyfiledialogs.h"


void Painter::loadColor1(){
    changeColor(color1);
}

void Painter::loadColor2(){
    changeColor(color2);
}

void Painter::loadColor3(){
    changeColor(color3);
}

void Painter::changeColor(Color &color){
    unsigned char defRGB[3];
    const char* check = tinyfd_colorChooser("Select a color", color.getHEX().c_str(),defRGB,defRGB);
    if(check)
        color.loadHex(check);
}

std::vector<glm::vec2> Painter::getCursorSubstitution(float spacing){
    
    glm::vec2 cursorPos = *Mouse::cursorPos();
    
    std::vector<glm::vec2> holdLocations;
    
    glm::vec2 fstrokeLocation = glm::vec2(cursorPos);
    holdLocations.push_back(fstrokeLocation);
    
    //----------------------PAINTING----------------------\\

    int differenceBetweenMousePoints = glm::distance(cursorPos, lastCursorPos);
    if(spacing > 5)
        differenceBetweenMousePoints = 1; 

    float xposDif = (cursorPos.x - lastCursorPos.x) / differenceBetweenMousePoints;
    float yposDif = (cursorPos.y - lastCursorPos.y) / differenceBetweenMousePoints;

    for (size_t i = 0; i < differenceBetweenMousePoints; i++)
    {
        ////differenceBetweenMousePoints > 10
        if (true) {
            cursorPos.x -= xposDif;
            cursorPos.y -= yposDif;
        }
        if(true){
            glm::vec2 strokeLocation = cursorPos;
            holdLocations.push_back(strokeLocation);
        }
    }
    return holdLocations;
}

Color Painter::getSelectedColor(){
    if(this->selectedColorIndex == 0)  
        return this->color1;
    if(this->selectedColorIndex == 1)  
        return this->color2;
    if(this->selectedColorIndex == 2)  
        return this->color3;
}

void Painter::applyVectorStrokes(Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos){
    
    int textureResolution = 256;

    Texture txtr = Texture(nullptr, textureResolution, textureResolution, GL_NEAREST);

    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Prepeare painting
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_ONE,GL_ONE);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);	

    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 

    for (size_t i = 0; i < this->vectorStrokes.size(); i++)
    {
        //ABS
        ShaderSystem::connectionCurve().use();
        
        ShaderSystem::connectionCurve().setMat4("projection", glm::ortho(0.f, (float)textureResolution, 0.f, (float)textureResolution));
        ShaderSystem::connectionCurve().setVec3("pos", glm::vec3(textureResolution/2.f, textureResolution/2.f, 0.9f));
        ShaderSystem::connectionCurve().setVec2("scale", glm::vec2(textureResolution/2.f, textureResolution/2.f));
        
        ShaderSystem::connectionCurve().setVec2("direction", this->vectorStrokes[i].offsetPos);
        
        ShaderSystem::connectionCurve().setVec2("startPos", this->vectorStrokes[i].startPos);
        ShaderSystem::connectionCurve().setVec2("destPos", this->vectorStrokes[i].endPos);
        ShaderSystem::connectionCurve().setVec2("percScale", glm::vec2(textureResolution));
        ShaderSystem::connectionCurve().setInt("lineCapturingMode", 1);
        
        ShaderSystem::connectionCurve().setFloat("EDGE", 0.0005);

        glDrawArrays(GL_TRIANGLES, 0 , 6);
    }

    //Finish
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);
    
    glDeleteFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    char* pixels = new char[txtr.getResolution().x * txtr.getResolution().y * 4];
    txtr.getData(pixels);

    std::vector<glm::vec2> strokePositions;

    for (size_t y = 0; y < txtr.getResolution().y; y++)
    {
        for (size_t x = 0; x < txtr.getResolution().x; x++){
            int pixelIndex = (y * txtr.getResolution().x + x) * 4; // Each pixel has 4 components (R, G, B, A)
            
            if(pixelIndex < txtr.getResolution().x * txtr.getResolution().y * 4){
                char alpha = pixels[pixelIndex + 3];
                
                if(alpha > 5){
                    strokePositions.push_back(glm::vec2(x * (Settings::videoScale()->x / textureResolution), y * (Settings::videoScale()->y / textureResolution)));
                }
            }
        }
    }

    const int paintingMode = 0;

    const int maxStrokeSize = 50;

    // Calculate how many subvectors you'll need
    const int numSubvectors = (strokePositions.size() + maxStrokeSize - 1) / maxStrokeSize;

    // Loop through and process the subvectors
    for (int i = 0; i < numSubvectors; ++i) {
        // Calculate the start and end indices for each subvector
        int startIdx = i * maxStrokeSize;
        int endIdx = std::min((i + 1) * maxStrokeSize, static_cast<int>(strokePositions.size()));

        // Create a subvector from the original vector
        std::vector<glm::vec2> subVector(strokePositions.begin() + startIdx, strokePositions.begin() + endIdx);

        // Call the imaginary function for the subvector
        this->doPaint(windowOrtho, subVector, paintingMode);
    
        for (size_t i = 0; i < subVector.size(); i++)
        {
            std::cout << glm::to_string(subVector[i]) << ' ';
        }
        
    }

    this->updateTexture(twoDPaintingPanel, windowOrtho, twoDSceneScroll, twoDScenePos, paintingMode);

    this->refreshPainting();
}

bool __anyVectoralPointMoving = false;
int __anyVectoralPointMovingType = 0;
int __anyVectoralPointMovingIndex = 0;

void VectorStroke::draw(float edge, bool sceneState, std::vector<VectorStroke>& strokes, int curI){
    glm::vec2 cursPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    const glm::vec2 dotScale = glm::vec2(10.);
    this->endPointHover = Mouse::isMouseHover(dotScale, this->endPos / 100.f * *Settings::videoScale());
    this->startPointHover = Mouse::isMouseHover(dotScale, this->startPos / 100.f * *Settings::videoScale());
    this->offsetPointHover = Mouse::isMouseHover(dotScale, this->offsetPos / 100.f * *Settings::videoScale());

    if(*Mouse::LClick()){
        if(endPointHover){
            this->endPointPressed = true;
            this->endPointClicked = true;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->endPointClicked = false;
        if(startPointHover){
            this->startPointPressed = true;
            this->startPointClicked = true;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->startPointClicked = false;
        if(offsetPointHover){
            this->offsetPointPressed = true;
            this->offsetPointClicked = true;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->offsetPointClicked = false;
    }

    if(!*Mouse::LPressed()){
        this->endPointPressed = false;
        this->startPointPressed = false;
        this->offsetPointPressed = false;
    }

    if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y){
        if(this->endPointPressed && !sceneState)
            this->endPointClicked = false;
        if(this->startPointPressed && !sceneState)
            this->startPointClicked = false;
        if(this->offsetPointPressed && !sceneState)
            this->offsetPointClicked = false;
    }

    if(this->endPointPressed){
        if(!__anyVectoralPointMoving || (__anyVectoralPointMovingIndex == curI + 1 && __anyVectoralPointMovingType == 1)){
            this->endPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 0;
            __anyVectoralPointMovingIndex = curI;
        }
    
        __anyVectoralPointMoving = true;
    }

    if(this->startPointPressed){
        if(!__anyVectoralPointMoving || (__anyVectoralPointMovingIndex == curI - 1 && __anyVectoralPointMovingType == 0)){
            this->startPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 1;
            __anyVectoralPointMovingIndex = curI;
        }
        
        __anyVectoralPointMoving = true;
    }

    if(this->offsetPointPressed){
        if(!__anyVectoralPointMoving){
            this->offsetPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 2;
            __anyVectoralPointMovingIndex = curI;
        }
        
        __anyVectoralPointMoving = true;
    }
        

    //ABS
    ShaderSystem::connectionCurve().use();
    
    ShaderSystem::connectionCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
    ShaderSystem::connectionCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
    
    ShaderSystem::connectionCurve().setVec2("direction", this->offsetPos);
    
    ShaderSystem::connectionCurve().setVec2("startPos", this->startPos);
    ShaderSystem::connectionCurve().setVec2("destPos", this->endPos);
    ShaderSystem::connectionCurve().setVec2("percScale", *Settings::videoScale());
    ShaderSystem::connectionCurve().setInt("lineCapturingMode", 0);
    
    ShaderSystem::connectionCurve().setFloat("EDGE", edge);

    if(endPointHover || endPointPressed)
        ShaderSystem::connectionCurve().setVec3("endPointColor", glm::vec3(0.f,1.f,0.f));
    else if(endPointClicked)
        ShaderSystem::connectionCurve().setVec3("endPointColor", glm::vec3(0.f,0.f,1.f));
    else
        ShaderSystem::connectionCurve().setVec3("endPointColor", glm::vec3(1.f,0.f,0.f));

    glDrawArrays(GL_TRIANGLES, 0 , 6);
    
    ShaderSystem::buttonShader().use();

    if(curI == 0)
        __anyVectoralPointMoving = false;
}