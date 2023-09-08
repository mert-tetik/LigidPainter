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

void VectorStroke::draw(float edge){
    glm::vec2 cursPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    const glm::vec2 dotScale = glm::vec2(10.);
    this->endPointHover = Mouse::isMouseHover(dotScale, this->endPos / 100.f * *Settings::videoScale());
    this->startPointHover = Mouse::isMouseHover(dotScale, this->startPos / 100.f * *Settings::videoScale());
    this->offsetPointHover = Mouse::isMouseHover(dotScale, this->offsetPos / 100.f * *Settings::videoScale());

    if(*Mouse::LClick()){
        if(endPointHover)
            this->endPointPressed = true;
        if(startPointHover)
            this->startPointPressed = true;
        if(offsetPointHover)
            this->offsetPointPressed = true;
    }

    if(!*Mouse::LPressed()){
        this->endPointPressed = false;
        this->startPointPressed = false;
        this->offsetPointPressed = false;
    }

    if(this->endPointPressed)
        this->endPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 

    if(this->startPointPressed)
        this->startPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 

    if(this->offsetPointPressed)
        this->offsetPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 

    //ABS
    ShaderSystem::connectionCurve().use();
    
    ShaderSystem::connectionCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
    ShaderSystem::connectionCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
    
    ShaderSystem::connectionCurve().setVec2("direction", this->offsetPos);
    
    ShaderSystem::connectionCurve().setVec2("startPos", this->startPos);
    ShaderSystem::connectionCurve().setVec2("destPos", this->endPos);
    ShaderSystem::connectionCurve().setVec2("percScale", glm::vec2(1.f));
    
    ShaderSystem::connectionCurve().setFloat("EDGE", edge);

    if(endPointHover)
        ShaderSystem::connectionCurve().setVec3("endPointColor", glm::vec3(0.f,1.f,0.f));
    else
        ShaderSystem::connectionCurve().setVec3("endPointColor", glm::vec3(1.f,0.f,0.f));

    glDrawArrays(GL_TRIANGLES, 0 , 6);
    
    ShaderSystem::buttonShader().use();
}