/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


#define TD_MODEL_FOLDER_CREATION 0
#define BRUSH_FOLDER_CREATION 1

//Forward declaration for the util functions
void completeFolder(std::string path, int action);


bool Project::createProject(std::string destinationPath, std::string name, std::string TDModelPath, int textureRes){
    
    //Make sure destination path doesn't have / at the end
    if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') 
        destinationPath.pop_back();

    //If the destination path doesn't exist give warning message box and return 
    if(!std::filesystem::exists(destinationPath)){
        showMessageBox(
                            "Warning!", 
                            "Project path is not valid! Please try another directory.", 
                            MESSAGEBOX_TYPE_WARNING, 
                            MESSAGEBOX_BUTTON_OK
                        );

        return false;
    }
    
    //If there is already a project with the same name and the same directory
    if(std::filesystem::exists(destinationPath + UTIL::folderDistinguisher() + name)){

        int res = showMessageBox
                (
                    "Warning!", 
                    "This folder is already exists. Do you want to overwrite?", 
                    MESSAGEBOX_TYPE_WARNING, 
                    MESSAGEBOX_BUTTON_YESNO
                );

        //If pressed the 'no' abort the process
        if(res == 0)
            return false;

        //If pressed to 'yes' remove the folder named same
        else if(res == 1)
            std::filesystem::remove_all(destinationPath + UTIL::folderDistinguisher() + name);
    }


    //Update the folder path of the project
    this->folderPath = destinationPath + UTIL::folderDistinguisher() + name;
    

    //Create the project folder
    if(!std::filesystem::create_directory(this->folderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << folderPath << std::endl; 

    //Create the textures folder
    std::string textureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures";
    if(!std::filesystem::create_directory(textureFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << textureFolderPath << std::endl; 

    //Materials
    std::string materialsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials";
    if(!std::filesystem::create_directory(materialsFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << materialsFolderPath << std::endl; 
    
    //Brushes
    std::string brushesFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes";
    if(!std::filesystem::create_directory(brushesFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << brushesFolderPath << std::endl; 
    
    //Fonts
    std::string fontsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Fonts";
    if(!std::filesystem::create_directory(fontsFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << fontsFolderPath << std::endl; 
    
    //Scripts
    std::string scriptsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Scripts";
    if(!std::filesystem::create_directory(scriptsFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << scriptsFolderPath << std::endl; 
    
    //Filters
    std::string filtersFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Filters";
    if(!std::filesystem::create_directory(filtersFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << filtersFolderPath << std::endl; 
    
    //Layers
    std::string layersFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Layers";
    if(!std::filesystem::create_directory(layersFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << layersFolderPath << std::endl; 
    
    //3D Models
    std::string tdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels";
    if(!std::filesystem::create_directory(tdModelFolderPath))
        std::cout << "ERROR : Creating project folder : Creating folder : " << tdModelFolderPath << std::endl; 
    completeFolder(tdModelFolderPath, TD_MODEL_FOLDER_CREATION);
    
    if(std::filesystem::exists(TDModelPath)){
        std::filesystem::copy(TDModelPath,tdModelFolderPath);
    }

    //Create the .ligid file
    writeLigidFile({}, textureRes);

    return true;
}

/// @brief add the contents off the related folder
void completeFolder(std::string path, int action){
    
    if(action == TD_MODEL_FOLDER_CREATION){
        std::filesystem::copy("./LigidPainter/Resources/3D Models/sphere.fbx", path + UTIL::folderDistinguisher() + "sphere.fbx");
        std::filesystem::copy("./LigidPainter/Resources/3D Models/plane.fbx", path + UTIL::folderDistinguisher() + "plane.fbx");
    }

}