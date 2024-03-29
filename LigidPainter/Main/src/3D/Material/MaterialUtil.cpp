/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
        this->element = nullptr;
        this->maskTxtr = nullptr;
        return;    
    }
    
    this->modI = newModI;

    if(this->element != nullptr){
        if(this->secI >= material.materialModifiers[newModI].sections.size()){
            std::cout << "ERROR : Update shortcut element : Invalid section index" << std::endl;
            this->element = nullptr;
            return;    
        }
        
        if(this->elementI >= material.materialModifiers[newModI].sections[secI].elements.size()){
            std::cout << "ERROR : Update shortcut element : Invalid element index" << std::endl;
            this->element = nullptr;
            return;    
        }
        
        this->element = &material.materialModifiers[newModI].sections[this->secI].elements[this->elementI];
    }
    else if(this->maskTxtr != nullptr){
        this->maskTxtr = &material.materialModifiers[newModI].maskTexture;
    }
}

bool Material::operator==(const Material& material) const{
    if(this->materialModifiers.size() != material.materialModifiers.size()){
        return false;
    }

    for (size_t modI = 0; modI < this->materialModifiers.size(); modI++)
    {
        if(this->materialModifiers[modI].maskTexture.ID != material.materialModifiers[modI].maskTexture.ID){
            return false;
        }

        if(this->materialModifiers[modI].maskTexture.proceduralProps != material.materialModifiers[modI].maskTexture.proceduralProps){
            return false;
        }
        
        if(this->materialModifiers[modI].modifierIndex != material.materialModifiers[modI].modifierIndex){
            return false;
        }
        
        if(this->materialModifiers[modI].shader.ID != material.materialModifiers[modI].shader.ID){
            return false;
        }
        
        if(this->materialModifiers[modI].shader.ID != material.materialModifiers[modI].shader.ID){
            return false;
        }
        
        if(this->materialModifiers[modI].hide != material.materialModifiers[modI].hide){
            return false;
        }
        
        if(this->materialModifiers[modI].sections.size() != material.materialModifiers[modI].sections.size()){
            return false;
        }

        for (size_t secI = 0; secI < this->materialModifiers[modI].sections.size(); secI++)
        {
            if(this->materialModifiers[modI].sections[secI] != material.materialModifiers[modI].sections[secI]){
                return false;
            }
        }   
    }

    return true;
}

bool Material::operator!=(const Material& material) const{
    return !(*this == material);
}