/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

std::vector<glm::vec2> Painter::getCursorSubstitution(float spacing, glm::vec2 start, glm::vec2 dest){
    
    glm::vec2 cursorPos = start;
    
    std::vector<glm::vec2> holdLocations;
    
    glm::vec2 fstrokeLocation = glm::vec2(cursorPos);
    holdLocations.push_back(fstrokeLocation);
    
    //----------------------PAINTING----------------------\\

    int differenceBetweenMousePoints = glm::distance(cursorPos, dest);
    if(spacing > 5)
        differenceBetweenMousePoints = 1; 

    float xposDif = (cursorPos.x - dest.x) / differenceBetweenMousePoints;
    float yposDif = (cursorPos.y - dest.y) / differenceBetweenMousePoints;

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

void Painter::applyVectorStrokes(
                                    std::vector<VectorStroke> vectorStrokes, 
                                    Panel& twoDPaintingPanel, 
                                    glm::mat4 windowOrtho, 
                                    int paintingMode, 
                                    Filter filterBtnFilter, 
                                    Box twoDPaintingBox, 
                                    Material& paintingCustomMat,
                                    std::vector<TextureField> textureFields
                                )
{
    
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

        LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Painter::applyVectorStrokes : Rendering vector strokes in line capturing mode");
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
        this->doPaint(windowOrtho, subVector, paintingMode, twoDPaintingPanel, twoDPaintingBox, true, textureFields);
    }

    this->updateTexture(twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, paintingCustomMat);

    this->refreshPainting();

    ShaderSystem::buttonShader().use();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
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

void Painter::updatePaintingOverTexture(std::vector<TextureField> textureFields){
    glm::ivec2 paintingRes = glm::vec2(this->getBufferResolutions(0));

    Framebuffer FBO = Framebuffer(this->paintingOverTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, paintingRes), "update painting over texture");
    FBO.bind();

    glViewport(0, 0, paintingRes.x, paintingRes.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering all the painting over texture fields
    for (int i = 0; i < textureFields.size(); i++)
    {
        if((this->wrapMode && textureFields[i].wrapMode) || !this->wrapMode){
            textureFields[i].render(Timer(), *this, textureFields, i, false, true, false);
        }
    }    

    // Finish
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    // Deleting the OpenGL framebuffer object & the renderbuffer object
    FBO.deleteBuffers(false, true);
}