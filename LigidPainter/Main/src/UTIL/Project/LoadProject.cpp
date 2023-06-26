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

bool Project::loadProject(std::string ligidFilePath,Library &library,Shaders shaders,Model &model,AppTextures appTextures,ColorPalette colorPalette, 
                          int& textureRes, std::vector<Node> &nodeScene , std::vector<Node> &appNodes){
    ;

    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        std::cout << "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << std::endl;
        return false;
    }

    //TODO Do smt with these variables
    time_t creationDate; 
    time_t lastOpenedDate;
    

    if(!readLigidFile(ligidFilePath, creationDate, lastOpenedDate, nodeScene, appNodes, textureRes)){
        std::cout << "ERROR CAN'T READ THE LIGID FILE : " << ligidFilePath << ". The file is might not be a ligid file." << std::endl;

        return false;
    }


    this->ligidFilePath = ligidFilePath;
    this->folderPath = UTIL::removeLastWordBySeparatingWithChar(ligidFilePath,folderDistinguisher);
    if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') //Make sure folder path doesn't have seperator at the end
        this->folderPath.pop_back();
    this->projectName = UTIL::getLastWordBySeparatingWithChar(folderPath,folderDistinguisher);

    
    //writeLigidFile(nodeScene);

    
    //Remove the / or \ from the projectName if there are any     
    if(projectName[projectName.size()-1] == '/' || projectName[projectName.size()-1] == '\\')
        projectName.pop_back();
    if(projectName[0] == '/' || projectName[0] == '\\')
        projectName.erase(projectName.begin());

    
    //Load the textures
    library.clearTextures();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Textures")){
        std::string texturePath = entry.path().string();

        Texture texture;
        texture.load(texturePath.c_str());

        library.addTexture(texture);
    }

    //Load the materials
    library.clearMaterials();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Materials")){
        std::string materialPath = entry.path().string();

        Material material(textureRes, "", 0);;
        material.readFile(materialPath,colorPalette,shaders.buttonShader,appTextures, library.materials);

        library.addMaterial(material);
    }
    
    //Load the brushes
    library.clearBrushes();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Brushes")){
        std::string brushPath = entry.path().string();

        Brush brush;
        brush.readFile(brushPath);
        brush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);


        library.addBrush(brush);
    }
    
    //Load the tdmodels
    library.clearModels();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "3DModels")){
        std::string modelPath = entry.path().string();

        Model TDModel;
        TDModel.loadModel(modelPath,true);

        library.addModel(TDModel);
    }

    if(library.TDModels.size())
        model = library.TDModels[0];

    return true;
}