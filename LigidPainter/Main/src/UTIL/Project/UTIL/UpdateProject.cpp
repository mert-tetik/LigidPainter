/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

static const std::string copyFolderTitle = "Updating_Folder-Remove_if_you_see";

// Forward declared utility functions
static void PROJECT_updateTextures(std::string textureFolderPath, bool recover_update_mode);
static void PROJECT_updateMaterials(std::string materialFolderPath);
static void PROJECT_updateBrushes(std::string brushFolderPath);
static void PROJECT_updateModels(std::string brushFolderPath);

// Goes from 1 to 3
static int currentRecoverSlot = 1;

void project_update(bool updateTextures, bool recover_update_mode){
    
    std::lock_guard<std::mutex> lock(project_mutex);

    // Check if the project path is valid
    if(!projectUTIL_folder_path_check()){
        LGDLOG::start<< "ERROR : CAN'T UPDATE THE PROJECT FOLDER : Project path is not valid : " << project_path() << LGDLOG::end;
        return;
    }
    
    // Process the destination path
    std::string destinationPath = project_path();
    if(recover_update_mode){
        destinationPath = project_recover_path(currentRecoverSlot);   
        
        currentRecoverSlot++;
        if(currentRecoverSlot == 4)
            currentRecoverSlot = 1;
        
        UTIL::createFolderIfDoesntExist(destinationPath);
    }

    //----------------- Textures
    if(updateTextures && !project_discard_update_flag){
        std::string textureFolderPath = destinationPath + UTIL::folderDistinguisher() + "Textures";
        UTIL::createFolderIfDoesntExist(textureFolderPath);
        PROJECT_updateTextures(textureFolderPath, recover_update_mode);
    }
    
    //----------------- Materials
    if(!project_discard_update_flag){
        std::string materialFolderPath = destinationPath + UTIL::folderDistinguisher() + "Materials";
        UTIL::createFolderIfDoesntExist(materialFolderPath);
        PROJECT_updateMaterials(materialFolderPath);
    }

    //----------------- Brushes
    if(!project_discard_update_flag){
        std::string brushFolderPath = destinationPath + UTIL::folderDistinguisher() + "Brushes";
        UTIL::createFolderIfDoesntExist(brushFolderPath);
        PROJECT_updateBrushes(brushFolderPath);
    }
    
    //----------------- Model
    if(!project_discard_update_flag){
        std::string modelFolderPath = destinationPath + UTIL::folderDistinguisher() + "3DModels";
        UTIL::createFolderIfDoesntExist(modelFolderPath);
        PROJECT_updateModels(modelFolderPath);
    }

    //----------------- Ligid
    if(!project_discard_update_flag){

        // Generate the path to the ligid file
        std::string lgdPath = project_locate_ligid_file(destinationPath); 
        if(lgdPath == ""){
            lgdPath = project_path() + UTIL::folderDistinguisher() + project_title() + ".ligid";
        }

        // Write the ligid file 
        if(!projectUTIL_write_ligid_file(lgdPath)){
            if(!recover_update_mode)
                LGDLOG::start << "Saving project folder : Failed to write the ligid file!" << LGDLOG::end; 
            else
                std::cout << "Writing recover files : Failed to write the ligid file!" << std::endl; 
        }
    }

    if(project_discard_update_flag)
        LGDLOG::start << "INFO : Updating project discarded" << LGDLOG::end;

    if(!recover_update_mode)
        LGDLOG::start << "Project saved successfuly" << LGDLOG::end;
    else
        std::cout << "Project saved successfuly" << std::endl;

    project_discard_update_flag = false;
}













// ---------------------------- UTILITY FUNCTIONS ---------------------------------


#define PROJECT_UPDATE_PROTOCOL_START(path)     std::string updateFolderPath = path + UTIL::folderDistinguisher() + copyFolderTitle;  \
                                                if(std::filesystem::exists(updateFolderPath)) \
                                                    std::filesystem::remove_all(updateFolderPath); \
                                                if(!std::filesystem::create_directory(updateFolderPath)){ \
                                                    LGDLOG::start << "ERROR : Updating project : faced with an issue while creating the update folder!" << LGDLOG::end; \
                                                    return; \
                                                }

#define PROJECT_UPDATE_PROTOCOL_END(path)   if(!project_discard_update_flag && !error){ \
                                                if(UTIL::deleteFilesInFolder(path)){\
                                                    UTIL::moveFilesToDestination(updateFolderPath, path);\
                                                    std::filesystem::remove_all(updateFolderPath);\
                                                }\
                                                else{\
                                                    LGDLOG::start << "WARNING! : Updating project : faced with an issue while deleting the updating folder" << LGDLOG::end;\
                                                }\
                                            }\
                                            else{\
                                                std::filesystem::remove_all(updateFolderPath);\
                                            }

static void PROJECT_updateModels(std::string modelFolderPath){

    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(modelFolderPath)

    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getModelArraySize(); i++)
    {
        if(!project_discard_update_flag){
            if(!FileHandler::writeLGDMODELFile(updateFolderPath, *Library::getModel(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a model file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(modelFolderPath)
}

static void PROJECT_updateBrushes(std::string brushFolderPath){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(brushFolderPath)
    
    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getBrushArraySize(); i++)
    {
        if(!project_discard_update_flag){
            if(!FileHandler::writeLGDBRUSHFile(updateFolderPath, Library::getBrushObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a brush file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(brushFolderPath)
}

static void PROJECT_updateMaterials(std::string materialFolderPath){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(materialFolderPath)

    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        if(!project_discard_update_flag){
            if(!FileHandler::writeLGDMATERIALFile(updateFolderPath, Library::getMaterialObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a material file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(materialFolderPath)
}

static void PROJECT_updateTextures(std::string textureFolderPath, bool recover_update_mode){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(textureFolderPath)

    bool error = false;

    // Write the textures
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            // Get the texture
            Texture txtr = Library::getTextureObj(i);

            if(!project_discard_update_flag){
                // Write texture data
                if(!txtr.exportTexture(updateFolderPath, "PNG")){
                    LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a texture file!" << LGDLOG::end;
                    error = true;
                }
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(textureFolderPath)
}