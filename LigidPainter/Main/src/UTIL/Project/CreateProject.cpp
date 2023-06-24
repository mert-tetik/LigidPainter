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

void completeFolder(std::string path, int action);

bool Project::createProject(std::string destinationPath,std::string name,std::string TDModelPath,int textureRes){
    
    if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') //Make sure destination path doesn't have seperator at the end
        destinationPath.pop_back();

    if(!std::filesystem::exists(destinationPath)){
        showMessageBox(
                            "Warning!", 
                            "Project path is not valid! Please try another directory.", 
                            MESSAGEBOX_TYPE_WARNING, 
                            MESSAGEBOX_BUTTON_OK
                        );

        return false;
    }
    
    if(std::filesystem::exists(destinationPath + folderDistinguisher + name)){

        int res = showMessageBox
                (
                    "Warning!", 
                    "This folder is already exists. Do you want to overwrite?", 
                    MESSAGEBOX_TYPE_WARNING, 
                    MESSAGEBOX_BUTTON_YESNO
                );

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
    
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    if(action == TD_MODEL_FOLDER_CREATION){
        std::filesystem::copy("./LigidPainter/Resources/3D Models/sphere.fbx", path + folderDistinguisher + "sphere.fbx");
        std::filesystem::copy("./LigidPainter/Resources/3D Models/plane.fbx", path + folderDistinguisher + "plane.fbx");
    }

}