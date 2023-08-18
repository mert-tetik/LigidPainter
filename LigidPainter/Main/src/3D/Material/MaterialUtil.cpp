/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Read *.lgdmaterial file

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

#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

Material Material::duplicateMaterial(){
    
    Material duplicatedMat(this->title + "_copied", 0);

    duplicatedMat.materialModifiers = this->materialModifiers;
    
    char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)

    for (size_t i = 0; i < duplicatedMat.materialModifiers.size(); i++)
    {
        duplicatedMat.materialModifiers[i].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
    }

    return duplicatedMat;
}
