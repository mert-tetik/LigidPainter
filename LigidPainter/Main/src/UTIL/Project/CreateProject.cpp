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


bool Project::createProject(std::string destinationPath,std::string name,std::string TDModelPath,int textureRes){
    if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') //Make sure destination path doesn't have seperator at the end
        destinationPath.pop_back();

    if(!std::filesystem::exists(destinationPath)){
        const char* title = "Warning";
        const char* message = "Project path is not valid! Please try another directory.";
        const char* icon = "warning";
        const char* button = "Ok";
        
        tinyfd_messageBox(title, message, button, icon, 1);
        return false;
    }
    if(!std::filesystem::exists(TDModelPath)){
        const char* title = "Warning";
        const char* message = "3D model path is not valid. Please try another 3D model file.";
        const char* icon = "warning";
        const char* button = "Ok";
        
        tinyfd_messageBox(title, message, button, icon, 1);
        return false;
    }
    if(std::filesystem::exists(destinationPath + folderDistinguisher + name)){
        const char* title = "Warning";
        const char* message = "This folder is already exists. Do you want to overwrite?";
        const char* icon = "warning";
        const char* button = "yesno";
        
        int res = tinyfd_messageBox(title, message, button, icon, 0);

        if(res == 0)
            return false;
        else if(res == 1)
            std::filesystem::remove_all(destinationPath + folderDistinguisher + name);
    }

    this->folderPath = destinationPath + folderDistinguisher + name;
    this->ligidFilePath = folderPath + folderDistinguisher + name + ".ligid";
    this->projectName = name;
    

    //Create the project folder
    std::filesystem::create_directory(this->folderPath);

    //Create the textures folder
    std::string textureFolderPath = this->folderPath + folderDistinguisher + "Textures";
    std::filesystem::create_directory(textureFolderPath);

    //Materials
    std::string materialsFolderPath = this->folderPath + folderDistinguisher + "Materials";
    std::filesystem::create_directory(materialsFolderPath);
    
    //Brushes
    std::string brushesFolderPath = this->folderPath + folderDistinguisher + "Brushes";
    std::filesystem::create_directory(brushesFolderPath);
    
    //Fonts
    std::string fontsFolderPath = this->folderPath + folderDistinguisher + "Fonts";
    std::filesystem::create_directory(fontsFolderPath);
    
    //Scripts
    std::string scriptsFolderPath = this->folderPath + folderDistinguisher + "Scripts";
    std::filesystem::create_directory(scriptsFolderPath);
    
    //Filters
    std::string filtersFolderPath = this->folderPath + folderDistinguisher + "Filters";
    std::filesystem::create_directory(filtersFolderPath);
    
    //Layers
    std::string layersFolderPath = this->folderPath + folderDistinguisher + "Layers";
    std::filesystem::create_directory(layersFolderPath);
    
    //3D Models
    std::string tdModelFolderPath = this->folderPath + folderDistinguisher + "3DModels";
    std::filesystem::create_directory(tdModelFolderPath);
    std::filesystem::copy(TDModelPath,tdModelFolderPath);

    //Create the .ligid file
    writeLigidFile({}, textureRes);

    return true;
}