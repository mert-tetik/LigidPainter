/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

#include <vector>
#include <string>


void Library::uniqueName(std::string &s,std::vector<std::string> sArray){
    bool isTheSame = false;
    for (size_t i = 0; i < sArray.size(); i++)
    {
        if(sArray[i] == s){
            isTheSame = true;
            this->changed = true;
        }
    }

    if(isTheSame){
        for (size_t i = 0; i < 1000; i++)
        {
            bool matched = false;
            for (size_t txtrI = 0; txtrI < sArray.size(); txtrI++)
            {
                if(sArray[txtrI] == s + '(' + std::to_string(i) + ')'){
                    matched = true;
                }
            }
            if(!matched){
                s += '(' + std::to_string(i) + ')';
                break;
            }
        }
    }
}

void Library::uniqueNameControl(){
    for (int i = textures.size()-1; i >= 0; i--)
    {
        std::vector<std::string> texturesStr;
        for (size_t istr = 0; istr < textures.size(); istr++)
        {
            if(i != istr)
                texturesStr.push_back(textures[istr].title);
        }

        uniqueName(textures[i].title,texturesStr);    
    }

    for (int i = materials.size()-1; i >= 0; i--)
    {
        std::vector<std::string> materialsStr;
        for (size_t istr = 0; istr < materials.size(); istr++)
        {
            if(i != istr)
                materialsStr.push_back(materials[istr].title);
        }

        uniqueName(materials[i].title,materialsStr);    
    }

    for (int i = brushes.size()-1; i >= 0; i--)
    {
        std::vector<std::string> brushesStr;
        for (size_t istr = 0; istr < brushes.size(); istr++)
        {
            if(i != istr)
                brushesStr.push_back(brushes[istr].title);
        }

        uniqueName(brushes[i].title,brushesStr);    
    }
    
    for (int i = TDModels.size()-1; i >= 0; i--)
    {
        std::vector<std::string> TDModelsStr;
        for (size_t istr = 0; istr < TDModels.size(); istr++)
        {
            if(i != istr)
                TDModelsStr.push_back(TDModels[istr].title);
        }

        uniqueName(TDModels[i].title,TDModelsStr);    
    }
}

/// @brief Add a texture to the textures vector
/// @param texture the texture
void Library::addTexture(Texture& texture){
    this->changed = true;
    
    texture.uniqueId = 0; 

    //Give unique ID to the texture
    Util util;
    util.giveUniqueId(texture.uniqueId,textures);

    this->textures.push_back(texture);
}

/// @brief Add a material to the materials vector
/// @param material the material
void Library::addMaterial(Material& material){
    this->changed = true;
    
    material.uniqueID = 0; 

    //Give unique ID to the material
    Util util;
    util.giveUniqueId(material.uniqueID,materials);

    this->materials.push_back(material);
}

/// @brief Add a brush to the brushes vector
/// @param brush the brush
void Library::addBrush(Brush& brush){
    this->changed = true;
    
    this->brushes.push_back(brush);
}

/// @brief Add a model to the tdmodels vector
/// @param model the model
void Library::addModel(Model& model){
    this->changed = true;
    
    this->TDModels.push_back(model);
}

/// @brief Delete a texture from the textures vector
/// @param index the texture corresponding to that index will be deleted
void Library::eraseTexture   (int index){
    this->changed = true;
    
    this->textures.erase(this->textures.begin() + index);
}

/// @brief Delete a material from the materials vector
/// @param index the material corresponding to that index will be deleted
void Library::eraseMaterial  (int index){
    this->changed = true;
    
    this->materials.erase(this->materials.begin() + index);
}

/// @brief Delete a brush from the brushes vector
/// @param index the brush corresponding to that index will be deleted
void Library::eraseBrush     (int index){
    this->changed = true;
    
    this->brushes.erase(this->brushes.begin() + index);
}

/// @brief Delete a model from the tdmodels vector
/// @param index the model corresponding to that index will be deleted
void Library::eraseModel     (int index){
    this->changed = true;
    
    this->TDModels.erase(this->TDModels.begin() + index);
}

/// @brief Clear the textures array 
void Library::clearTextures   (){
    this->changed = true;
    
    this->textures.clear();
}

/// @brief Clear the materials array 
void Library::clearMaterials  (){
    this->changed = true;
    
    this->materials.clear();
}

/// @brief Clear the brushes array 
void Library::clearBrushes     (){
    this->changed = true;
    
    this->brushes.clear();
}

/// @brief Clear the TDModels array 
void Library::clearModels     (){
    this->changed = true;
    
    this->TDModels.clear();
}

void Library::changeSelectedElementIndex(int newI){
    this->changed = true;
    
    this->selectedElementIndex = newI;
}