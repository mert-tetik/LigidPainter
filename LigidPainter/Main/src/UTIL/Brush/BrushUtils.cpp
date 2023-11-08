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
    
void Brush::useBrush(Section &brushSection){
    brushSection.elements[3].rangeBar.value        =   this->hardness;
    brushSection.elements[4].rangeBar.value        =   this->spacing;
    brushSection.elements[5].rangeBar.value        =   this->sizeJitter;
    brushSection.elements[8].rangeBar.value        =   this->scatter;
    brushSection.elements[6].rangeBar.value        =   this->fade;
    brushSection.elements[12].rangeBar.value        =   this->rotation;
    brushSection.elements[13].rangeBar.value        =   this->rotationJitter;
    brushSection.elements[14].rangeBar.value        =   this->alphaJitter;
    brushSection.elements[11].checkBox.clickState1  =   this->individualTexture;
    brushSection.elements[7].checkBox.clickState1  =   this->sinWavePattern;
    brushSection.elements[9].button.texture.proceduralProps = this->texture.proceduralProps;
    brushSection.elements[9].button.texture.generateProceduralDisplayingTexture(512, false);
    
}

void Brush::applyToBrush(Section &brushSection){
    this->hardness         =   brushSection.elements[3].rangeBar.value;          
    this->spacing         =   brushSection.elements[4].rangeBar.value;          
    this->sizeJitter         =   brushSection.elements[5].rangeBar.value;          
    this->scatter            =   brushSection.elements[8].rangeBar.value;          
    this->fade               =   brushSection.elements[6].rangeBar.value;          
    this->rotation           =   brushSection.elements[12].rangeBar.value;          
    this->rotationJitter     =   brushSection.elements[13].rangeBar.value;          
    this->alphaJitter        =   brushSection.elements[14].rangeBar.value;          
    this->individualTexture  =   brushSection.elements[11].checkBox.clickState1;    
    this->sinWavePattern     =   brushSection.elements[7].checkBox.clickState1;    
    this->texture.proceduralProps = brushSection.elements[9].button.texture.proceduralProps;
    this->texture.generateProceduralDisplayingTexture(512, false);

    updateDisplayTexture(0.1);
}