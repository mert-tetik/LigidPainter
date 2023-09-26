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
    
    Material duplicatedMat(this->title + "_duplicated", 0);

    duplicatedMat.materialModifiers = this->materialModifiers;

    for (size_t modI = 0; modI < duplicatedMat.materialModifiers.size(); modI++)
    {
        for (size_t secI = 0; secI < duplicatedMat.materialModifiers[modI].sections.size(); secI++)
        {
            for (size_t elI = 0; elI < duplicatedMat.materialModifiers[modI].sections[secI].elements.size(); elI++)
            {
                if(duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].state == 0){
                    if(duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].button.texture.ID){
                        duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].button.texture = this->materialModifiers[modI].sections[secI].elements[elI].button.texture.duplicateTexture();
                    }
                }
            }
        }
    }
    
    for (size_t i = 0; i < duplicatedMat.materialModifiers.size(); i++)
    {
        duplicatedMat.materialModifiers[i].maskTexture = this->materialModifiers[i].maskTexture;
        duplicatedMat.materialModifiers[i].maskTexture = this->materialModifiers[i].maskTexture.duplicateTexture();
    }

    return duplicatedMat;
}
