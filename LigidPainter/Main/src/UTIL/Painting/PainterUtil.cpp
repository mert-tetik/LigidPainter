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

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

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

void Painter::applyVectorStrokes(std::vector<VectorStroke> vectorStrokes, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox){
    
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

    for (size_t i = 0; i < vectorStrokes.size(); i++)
    {
        //ABS
        ShaderSystem::vectoralCurve().use();
        
        ShaderSystem::vectoralCurve().setMat4("projection", glm::ortho(0.f, (float)textureResolution, 0.f, (float)textureResolution));
        ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(textureResolution/2.f, textureResolution/2.f, 0.9f));
        ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(textureResolution/2.f, textureResolution/2.f));
        
        ShaderSystem::vectoralCurve().setVec2("direction", vectorStrokes[i].offsetPos);
        
        ShaderSystem::vectoralCurve().setVec2("startPos", vectorStrokes[i].startPos);
        ShaderSystem::vectoralCurve().setVec2("destPos", vectorStrokes[i].endPos);
        ShaderSystem::vectoralCurve().setVec2("percScale", glm::vec2(textureResolution));
        ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 1);
        
        ShaderSystem::vectoralCurve().setFloat("EDGE", 0.0005);

        glDrawArrays(GL_TRIANGLES, 0 , 6);
    }

    //Finish
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    glDepthFunc(GL_LESS);
    
    glDeleteFramebuffers(1, &FBO);
    Settings::defaultFramebuffer()->FBO.bind();

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

        // Call the function for the subvector
        this->doPaint(windowOrtho, subVector, paintingMode, twoDPaintingPanel, twoDPaintingBox);
    }

    this->updateTexture(twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox);

    this->refreshPainting();

    ShaderSystem::buttonShader().use();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

bool __anyVectoralPointMoving = false;
int __anyVectoralPointMovingType = 0;
int __anyVectoralPointMovingIndex = 0;

void VectorStroke::draw(Timer& timer, float edge, bool sceneState, std::vector<VectorStroke>& strokes, int curI){
    glm::vec2 cursPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    const glm::vec2 dotScale = glm::vec2(10.);
    this->endPointHover = Mouse::isMouseHover(dotScale, this->endPos / 100.f * *Settings::videoScale());
    this->startPointHover = Mouse::isMouseHover(dotScale, this->startPos / 100.f * *Settings::videoScale());
    this->offsetPointHover = Mouse::isMouseHover(dotScale, this->offsetPos / 100.f * *Settings::videoScale());

    if(*Mouse::LClick()){
        if(endPointHover){
            if(this->startPos != this->endPos)
                this->endPointPressed = true;
            this->endPointClicked = !this->endPointClicked;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->endPointClicked = false;
        if(startPointHover){
            if(this->startPos != this->endPos)
                this->startPointPressed = true;
            this->startPointClicked = !this->startPointClicked;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->startPointClicked = false;
        if(offsetPointHover){
            if(this->startPos != this->endPos)
                this->offsetPointPressed = true;
            this->offsetPointClicked = !this->offsetPointClicked;
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
    ShaderSystem::vectoralCurve().use();
    
    ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
    ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
    
    ShaderSystem::vectoralCurve().setVec2("direction", this->offsetPos);
    
    ShaderSystem::vectoralCurve().setVec2("startPos", this->startPos);
    ShaderSystem::vectoralCurve().setVec2("destPos", this->endPos);
    ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
    ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);
    
    ShaderSystem::vectoralCurve().setFloat("EDGE", edge);

    glm::vec3 endPointColor = glm::vec3(1.f, 1.f, 1.f);

    if(endPointHover || endPointPressed)
        endPointColor = ColorPalette::themeColor;
    else if(endPointClicked)
        endPointColor = glm::vec3(0.f,0.f,1.f);

    glDrawArrays(GL_TRIANGLES, 0 , 6);
    
    ShaderSystem::buttonShader().use();

    Button endPointBtn = Button(0, glm::vec2(1.f), "", Texture(), 0.f, true);
    endPointBtn.hoverMixVal = endPointHoverMixVal;
    endPointBtn.clickedMixVal = endPointClickedMixVal;
    endPointBtn.hover = endPointHover;
    endPointBtn.clickState1 = endPointClicked;
    endPointBtn.radius = 0.3f;
    endPointBtn.scale = glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y) / 2.f;
    endPointBtn.pos = glm::vec3(this->endPos, 0.9f);

    Button startPointBtn = endPointBtn;
    startPointBtn.pos = glm::vec3(this->startPos, 0.9f);
    startPointBtn.clickState1 = startPointClicked;
    startPointBtn.hover = startPointHover;
    startPointBtn.clickedMixVal = startPointClickedMixVal;
    startPointBtn.hoverMixVal = startPointHoverMixVal;
    
    if(edge == 0.0001f){
        endPointBtn.scale /= 2.;
        endPointBtn.radius = 0.2;
    }
    endPointBtn.render(timer, true);
    if(edge != 0.0001f)
        startPointBtn.render(timer, true);

    if(startPointHover || startPointPressed || endPointHover || endPointPressed || offsetPointHover || offsetPointPressed){
        Mouse::setCursor(*Mouse::pointerCursor());
    }

    endPointHoverMixVal = endPointBtn.hoverMixVal;
    endPointClickedMixVal = endPointBtn.clickedMixVal;
    startPointHoverMixVal = startPointBtn.hoverMixVal;
    startPointClickedMixVal = startPointBtn.clickedMixVal;

    if(curI == 0)
        __anyVectoralPointMoving = false;
}

void Painter::refreshBuffers(){
    glm::ivec2 paintingRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier);
    glm::ivec2 depthRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingDepthTextureResolutionDivier);

    //--------- init paintingTexture8 --------- 
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture8);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, paintingRes.x, paintingRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //--------- init paintingTexture16f --------- 
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture16f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, paintingRes.x, paintingRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //--------- init depthTextures --------- 
    this->oSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oXSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oYSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oXYSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oZSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oXZSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oYZSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    
    this->oXYZSide.depthTexture.update(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);

    //--------- init paintingOverTexture --------- 
    glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture);
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, paintingRes.x, paintingRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //--------- init paintingFBO --------- 
    glBindFramebuffer(GL_FRAMEBUFFER, this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
    
    //--------- init depthRBO --------- 
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, depthRes.x, depthRes.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    //--------- Finish --------- 
    Settings::defaultFramebuffer()->FBO.bind();
}

int Painter::getSelectedTextureIndexInLibrary(){
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(this->selectedTexture.ID == Library::getTexture(i)->ID)
            return i;
    }
    
    return -1;
}


glm::mat4 MirrorSide::getViewMat(glm::vec3 offset){

    offset *= glm::max(this->effectAxis, 0.f);

    glm::vec3 orgCamPos = getScene()->camera.cameraPos;
    glm::vec3 orgOriginPos = getScene()->camera.originPos;
    glm::vec3 camPosOriginDistance = orgCamPos - orgOriginPos;
    glm::vec3 camPos = orgOriginPos * -this->effectAxis - camPosOriginDistance * this->effectAxis - offset * 2.f; 
    return getScene()->calculateViewMatrix(camPos, orgOriginPos * -this->effectAxis - offset * 2.f);
}

void Painter::setBrushProperties(Section brushSection){
    this->brushProperties.radius = brushSection.elements[1].rangeBar.value;
    this->brushProperties.opacity = brushSection.elements[2].rangeBar.value;
    this->brushProperties.hardness = brushSection.elements[3].rangeBar.value;
    this->brushProperties.spacing = brushSection.elements[4].rangeBar.value;
    this->brushProperties.sizeJitter = brushSection.elements[5].rangeBar.value;
    this->brushProperties.fade = brushSection.elements[6].rangeBar.value;
    this->brushProperties.sinWavePattern = brushSection.elements[7].checkBox.clickState1;
    this->brushProperties.scatter = brushSection.elements[8].rangeBar.value;
    this->brushProperties.brushTexture = brushSection.elements[9].button.texture;
    this->brushProperties.individualTexture = brushSection.elements[11].checkBox.clickState1;
    this->brushProperties.rotation = brushSection.elements[12].rangeBar.value;
    this->brushProperties.rotationJitter = brushSection.elements[13].rangeBar.value;
    this->brushProperties.alphaJitter = brushSection.elements[14].rangeBar.value;
}