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

#include <vector>
#include <string>


void Library::uniqueName(std::string &s,std::vector<std::string> sArray){
    bool isTheSame = false;
    for (size_t i = 0; i < sArray.size(); i++)
    {
        if(sArray[i] == s){
            isTheSame = true;
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
    for (size_t i = 0; i < textures.size(); i++)
    {
        std::vector<std::string> texturesStr;
        for (size_t istr = 0; istr < textures.size(); istr++)
        {
            if(i != istr)
                texturesStr.push_back(textures[istr].title);
        }

        uniqueName(textures[i].title,texturesStr);    
    }

    for (size_t i = 0; i < materials.size(); i++)
    {
        std::vector<std::string> materialsStr;
        for (size_t istr = 0; istr < materials.size(); istr++)
        {
            if(i != istr)
                materialsStr.push_back(materials[istr].title);
        }

        uniqueName(materials[i].title,materialsStr);    
    }

    for (size_t i = 0; i < brushes.size(); i++)
    {
        std::vector<std::string> brushesStr;
        for (size_t istr = 0; istr < brushes.size(); istr++)
        {
            if(i != istr)
                brushesStr.push_back(brushes[istr].title);
        }

        uniqueName(brushes[i].title,brushesStr);    
    }
    
    for (size_t i = 0; i < TDModels.size(); i++)
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

void Library::addTexture(Texture& texture){
    texture.uniqueId = 0; 

    //Give unique ID to the texture
    Util util;
    util.giveUniqueId(texture.uniqueId,textures);

    this->textures.push_back(texture);
}
void Library::addMaterial(Material& material){
    material.uniqueID = 0; 

    //Give unique ID to the material
    Util util;
    util.giveUniqueId(material.uniqueID,materials);

    this->materials.push_back(material);
}
void Library::addBrush(Brush& brush){
    this->brushes.push_back(brush);
}
void Library::addModel(Model& model){
    this->TDModels.push_back(model);
}