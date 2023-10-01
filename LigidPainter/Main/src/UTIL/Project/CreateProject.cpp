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
#define FILTER_FOLDER_CREATION 2

//Forward declaration for the util functions
void completeFolder(std::string path, int action);


bool Project::createProject(std::string destinationPath, std::string name, std::string TDModelPath, int textureRes){

    while(true){
        if(!this->projectProcessing)
            break;
    }

    this->projectProcessing = true;
    
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
        else if(res == 1){
            try
            {
                std::filesystem::remove_all(destinationPath + UTIL::folderDistinguisher() + name);
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LGDLOG::start << "ERROR : Filesystem : Location ID 286424 " << ex.what() << LGDLOG::end;
            }
        }
    }


    //Update the folder path of the project
    this->folderPath = destinationPath + UTIL::folderDistinguisher() + name;
    
    try
    {
        //Create the project folder
        if(!std::filesystem::create_directory(this->folderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << folderPath << LGDLOG::end; 

        //Create the saved data folder
        std::string savedFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Saved";
        if(!std::filesystem::create_directory(savedFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << savedFolderPath << LGDLOG::end; 
        
        //Create the textures folder
        std::string textureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures";
        if(!std::filesystem::create_directory(textureFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << textureFolderPath << LGDLOG::end; 

        //Materials
        std::string materialsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials";
        if(!std::filesystem::create_directory(materialsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << materialsFolderPath << LGDLOG::end; 
        
        //Brushes
        std::string brushesFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes";
        if(!std::filesystem::create_directory(brushesFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << brushesFolderPath << LGDLOG::end; 
        completeFolder(brushesFolderPath, BRUSH_FOLDER_CREATION);
        
        //Fonts
        std::string fontsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Fonts";
        if(!std::filesystem::create_directory(fontsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << fontsFolderPath << LGDLOG::end; 
        
        //Scripts
        std::string scriptsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Scripts";
        if(!std::filesystem::create_directory(scriptsFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << scriptsFolderPath << LGDLOG::end; 
        
        //Texture Packs
        std::string texturePacksFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Texture Packs";
        if(!std::filesystem::create_directory(texturePacksFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << texturePacksFolderPath << LGDLOG::end; 
        
        //Filters
        std::string filtersFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Filters";
        if(!std::filesystem::create_directory(filtersFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << filtersFolderPath << LGDLOG::end; 
        completeFolder(filtersFolderPath, FILTER_FOLDER_CREATION);
        
        //Layers
        std::string layersFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Layers";
        if(!std::filesystem::create_directory(layersFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << layersFolderPath << LGDLOG::end; 
        
        //3D Models
        std::string tdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels";
        if(!std::filesystem::create_directory(tdModelFolderPath))
            LGDLOG::start<< "ERROR : Creating project folder : Creating folder : " << tdModelFolderPath << LGDLOG::end; 
        completeFolder(tdModelFolderPath, TD_MODEL_FOLDER_CREATION);
        
        if(std::filesystem::exists(TDModelPath)){
            std::filesystem::copy(TDModelPath,tdModelFolderPath);
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 111121 " << ex.what() << LGDLOG::end;
    }

    //Create the .ligid file
    writeLigidFile();

    this->projectProcessing = false;

    return true;
}

/// @brief add the contents off the related folder
void completeFolder(std::string path, int action){
    
    if(action == TD_MODEL_FOLDER_CREATION){
        std::filesystem::copy("./LigidPainter/Resources/3D Models/sphere.fbx", path + UTIL::folderDistinguisher() + "sphere.fbx");
        std::filesystem::copy("./LigidPainter/Resources/3D Models/plane.fbx", path + UTIL::folderDistinguisher() + "plane.fbx");
    }
    else if(action == FILTER_FOLDER_CREATION){
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Filters")) {
            if (std::filesystem::is_regular_file(entry)) {
                // Get the filename from the full path
                std::string filename = entry.path().filename().string();
                
                // Create the destination path by combining the destination folder path and filename
                std::filesystem::path destinationPath = std::filesystem::path(path) / filename;
                
                // Copy the file
                std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
    else if(action == BRUSH_FOLDER_CREATION){
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Brushes")) {
            if (std::filesystem::is_regular_file(entry)) {
                // Get the filename from the full path
                std::string filename = entry.path().filename().string();
                
                // Create the destination path by combining the destination folder path and filename
                std::filesystem::path destinationPath = std::filesystem::path(path) / filename;
                
                // Copy the file
                std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
}