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

static Mesh emptyMesh;
Mesh* Painter::getSelectedMesh(){
    if(this->selectedMeshIndex < getModel()->meshes.size())
        return &getModel()->meshes[this->selectedMeshIndex];

    return &emptyMesh;
}