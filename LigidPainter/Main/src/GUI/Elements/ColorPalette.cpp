/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Color Pallete of the LigidPainter.

These color values are determining the colors of the graphical user interface & has nothing to do with any other color palette implementations


*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements/Elements.hpp"

ColorPalette::ColorPalette(){}

void ColorPalette::newPalette(
                    glm::vec4 themeColor,
                    glm::vec4 mainColor,
                    glm::vec4 secondColor,
                    glm::vec4 thirdColor,
                    glm::vec4 oppositeColor
                ){
    
    this->themeColor = themeColor;
    this->mainColor = mainColor;
    this->secondColor = secondColor;
    this->thirdColor = thirdColor;
    this->oppositeColor = oppositeColor;
}