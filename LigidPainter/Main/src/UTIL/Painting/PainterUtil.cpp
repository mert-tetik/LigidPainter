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
    std::vector<glm::vec2> holdLocations;
    
    glm::vec2 fstrokeLocation = glm::vec2(*Mouse::cursorPos());
    holdLocations.push_back(fstrokeLocation);
    
    //----------------------PAINTING----------------------\\

    int differenceBetweenMousePoints = glm::distance(*Mouse::cursorPos(), lastCursorPos);
    if(spacing > 5)
        differenceBetweenMousePoints = 1; 

    float xposDif = (Mouse::cursorPos()->x - lastCursorPos.x) / differenceBetweenMousePoints;
    float yposDif = (Mouse::cursorPos()->y - lastCursorPos.y) / differenceBetweenMousePoints;

    for (size_t i = 0; i < differenceBetweenMousePoints; i++)
    {
        ////differenceBetweenMousePoints > 10
        if (true) {
            Mouse::cursorPos()->x -= xposDif;
            Mouse::cursorPos()->y -= yposDif;
        }
        if(true){
            glm::vec2 strokeLocation = *Mouse::cursorPos();
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