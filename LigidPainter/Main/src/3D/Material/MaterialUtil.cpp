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
    duplicatedMat.materialShortcuts = this->materialShortcuts;
    for (size_t i = 0; i < duplicatedMat.materialShortcuts.size(); i++)
    {
        duplicatedMat.materialShortcuts[i].updateElement(duplicatedMat, duplicatedMat.materialShortcuts[i].modI);
    }
    

    for (size_t modI = 0; modI < duplicatedMat.materialModifiers.size(); modI++)
    {
        for (size_t secI = 0; secI < duplicatedMat.materialModifiers[modI].sections.size(); secI++)
        {
            for (size_t elI = 0; elI < duplicatedMat.materialModifiers[modI].sections[secI].elements.size(); elI++)
            {
                if(duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].state == 0){
                    if(duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].button.texture.ID){
                        duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].button.texture.ID = this->materialModifiers[modI].sections[secI].elements[elI].button.texture.duplicateTexture();
                        duplicatedMat.materialModifiers[modI].sections[secI].elements[elI].button.texture.proceduralProps = this->materialModifiers[modI].sections[secI].elements[elI].button.texture.proceduralProps;
                    }
                }
            }
        }
    }
    
    for (size_t i = 0; i < duplicatedMat.materialModifiers.size(); i++)
    {
        duplicatedMat.materialModifiers[i].maskTexture.ID = this->materialModifiers[i].maskTexture.duplicateTexture();
        duplicatedMat.materialModifiers[i].maskTexture.proceduralProps = this->materialModifiers[i].maskTexture.proceduralProps;
    }

    duplicatedMat.updateMaterialDisplayingTexture(128, true, Camera(), 0, false);

    return duplicatedMat;
}

void Material::deleteBuffers(){
    this->displayingFBO.deleteBuffers(true, true);
    glDeleteTextures(1, &this->displayingTexture.ID);
    
    for (size_t i = 0; i < this->materialModifiers.size(); i++)
    {
        glDeleteTextures(1, &this->materialModifiers[i].maskTexture.ID);
        
        for (size_t seci = 0; seci < this->materialModifiers[i].sections.size(); seci++)
        {
            for (size_t eli = 0; eli < this->materialModifiers[i].sections[seci].elements.size(); eli++)
            {
                glDeleteTextures(1, &this->materialModifiers[i].sections[seci].elements[eli].button.texture.ID);
            }
        }
    }
}

void MaterialShortcut::updateElement(Material &material, int newModI){
    if(newModI >= material.materialModifiers.size()){
        std::cout << "ERROR : Update shortcut element : Invalid modifier index" << std::endl;
        return;    
    }
    
    if(this->secI >= material.materialModifiers[newModI].sections.size()){
        std::cout << "ERROR : Update shortcut element : Invalid section index" << std::endl;
        return;    
    }
    
    if(this->elementI >= material.materialModifiers[newModI].sections[secI].elements.size()){
        std::cout << "ERROR : Update shortcut element : Invalid element index" << std::endl;
        return;    
    }

    this->element = &material.materialModifiers[newModI].sections[this->secI].elements[this->elementI];
}