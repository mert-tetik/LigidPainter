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

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#include "tinyfiledialogs.h"

bool Project::loadProject(std::string ligidFilePath,Library &library,Shaders shaders,Model &model,AppTextures appTextures,ColorPalette colorPalette){
    Util util;

    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        std::cout << "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << std::endl;
        return false;
    }

    std::ifstream rf(ligidFilePath, std::ios::out | std::ios::binary);
		
    //Return if can't open the ligidFilePath
    if(!rf) {
        std::cout << "ERROR WHILE READING THE LIGID FILE! Cannot open file : " << ligidFilePath << std::endl;
        return false;
    }

    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 
    
    uint64_t ch1; 
    uint64_t ch2; 
    uint64_t ch3; 
    
    //Read the description of the ligid file
    rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
    rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
    rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));
    
    //If the description doesn't matches
    if(ch1 != h1 || ch2 != h2 || ch3 != h3){
        std::cout << "ERROR THIS IS NOT A LIGID FILE! Description header doesn't match : " << ligidFilePath << std::endl;
        return false;
    }

    this->ligidFilePath = ligidFilePath;
    this->folderPath = util.removeLastWordBySeparatingWithChar(ligidFilePath,folderDistinguisher);
    if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') //Make sure folder path doesn't have seperator at the end
        this->folderPath.pop_back();
    this->projectName = util.getLastWordBySeparatingWithChar(folderPath,folderDistinguisher);

    
    updateLigidFile();

    
    //Remove the / or \ from the projectName if there are any     
    if(projectName[projectName.size()-1] == '/' || projectName[projectName.size()-1] == '\\')
        projectName.pop_back();
    if(projectName[0] == '/' || projectName[0] == '\\')
        projectName.erase(projectName.begin());

    
    //Load the textures
    library.textures.clear();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Textures")){
        std::string texturePath = entry.path().string();

        Texture texture;
        texture.load(texturePath.c_str());

        library.textures.push_back(texture);
    }

    //Load the materials
    library.materials.clear();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Materials")){
        std::string materialPath = entry.path().string();

        Material material;
        material.readFile(materialPath,colorPalette,shaders.buttonShader,appTextures);

        library.materials.push_back(material);
    }
    
    //Load the brushes
    library.brushes.clear();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Brushes")){
        std::string brushPath = entry.path().string();

        Brush brush;
        brush.readFile(brushPath);
        brush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);


        library.brushes.push_back(brush);
    }
    
    //Load the tdmodels
    library.TDModels.clear();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "3DModels")){
        std::string modelPath = entry.path().string();

        Model TDModel;
        TDModel.loadModel(modelPath,true);

        library.TDModels.push_back(TDModel);
    }

    if(library.TDModels.size())
        model = library.TDModels[0];

    return true;
}