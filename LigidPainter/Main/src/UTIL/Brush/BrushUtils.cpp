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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"

#include "GUI/GUI.hpp"
    
void Brush::useBrush(Panel &paintingPanel){
    paintingPanel.sections[1].elements[3].rangeBar.value        =   this->hardness;
    paintingPanel.sections[1].elements[4].rangeBar.value        =   this->spacing;
    paintingPanel.sections[1].elements[5].rangeBar.value        =   this->sizeJitter;
    paintingPanel.sections[1].elements[8].rangeBar.value        =   this->scatter;
    paintingPanel.sections[1].elements[6].rangeBar.value        =   this->fade;
    paintingPanel.sections[1].elements[12].rangeBar.value        =   this->rotation;
    paintingPanel.sections[1].elements[13].rangeBar.value        =   this->rotationJitter;
    paintingPanel.sections[1].elements[14].rangeBar.value        =   this->alphaJitter;
    paintingPanel.sections[1].elements[11].checkBox.clickState1  =   this->individualTexture;
    paintingPanel.sections[1].elements[7].checkBox.clickState1  =   this->sinWavePattern;
    paintingPanel.sections[1].elements[9].button.texture.proceduralProps = this->texture.proceduralProps;
    paintingPanel.sections[1].elements[9].button.texture.generateProceduralDisplayingTexture(512);
}

void Brush::applyToBrush(Panel &paintingPanel){
    this->hardness         =   paintingPanel.sections[1].elements[3].rangeBar.value;          
    this->spacing         =   paintingPanel.sections[1].elements[4].rangeBar.value;          
    this->sizeJitter         =   paintingPanel.sections[1].elements[5].rangeBar.value;          
    this->scatter            =   paintingPanel.sections[1].elements[8].rangeBar.value;          
    this->fade               =   paintingPanel.sections[1].elements[6].rangeBar.value;          
    this->rotation           =   paintingPanel.sections[1].elements[12].rangeBar.value;          
    this->rotationJitter     =   paintingPanel.sections[1].elements[13].rangeBar.value;          
    this->alphaJitter        =   paintingPanel.sections[1].elements[14].rangeBar.value;          
    this->individualTexture  =   paintingPanel.sections[1].elements[11].checkBox.clickState1;    
    this->sinWavePattern     =   paintingPanel.sections[1].elements[7].checkBox.clickState1;    
    this->texture.proceduralProps = paintingPanel.sections[1].elements[9].button.texture.proceduralProps;
    this->texture.generateProceduralDisplayingTexture(512);

    updateDisplayTexture(0.1);
}