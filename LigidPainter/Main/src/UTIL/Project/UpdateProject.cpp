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

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "LibrarySystem/Library.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

static const std::string copyFolderTitle = "Updating_Folder-Remove_if_you_see";

// Forward declared utility functions
static void PROJECT_updateTextures(std::string textureFolderPath, bool multithreadingMode, bool& discardUpdateProjectFlag);
static void PROJECT_updateMaterials(std::string materialFolderPath, bool& discardUpdateProjectFlag);
static void PROJECT_updateBrushes(std::string brushFolderPath, bool& discardUpdateProjectFlag);
static void PROJECT_updateModels(std::string brushFolderPath, bool& discardUpdateProjectFlag);

// Goes from 1 to 3
static int currentRecoverSlot = 1;

void Project::updateProject(bool updateTextures, bool multithreadingMode){
    
    // Multithreading compat
    while(true){
        if(!this->projectProcessing)
            break;
    }
    this->projectProcessing = true;

    // Check if the project path is valid
    if(!this->folderPathCheck()){
        LGDLOG::start<< "ERROR : CAN'T UPDATE THE PROJECT FOLDER : Project path is not valid : " << this->folderPath << LGDLOG::end;
        this->projectProcessing = false;
        return;
    }
    
    // Process the destination path
    std::string destinationPath = this->folderPath;
    if(multithreadingMode){
        destinationPath = this->recoverSlotPath(currentRecoverSlot);   
        
        currentRecoverSlot++;
        if(currentRecoverSlot == 4)
            currentRecoverSlot = 1;
        
        UTIL::createFolderIfDoesntExist(destinationPath);
    }

    //----------------- Textures
    if(updateTextures && !discardUpdateProjectFlag){
        std::string textureFolderPath = destinationPath + UTIL::folderDistinguisher() + "Textures";
        UTIL::createFolderIfDoesntExist(textureFolderPath);
        PROJECT_updateTextures(textureFolderPath, multithreadingMode, this->discardUpdateProjectFlag);
    }
    
    //----------------- Materials
    if(!discardUpdateProjectFlag){
        std::string materialFolderPath = destinationPath + UTIL::folderDistinguisher() + "Materials";
        UTIL::createFolderIfDoesntExist(materialFolderPath);
        PROJECT_updateMaterials(materialFolderPath, this->discardUpdateProjectFlag);
    }

    //----------------- Brushes
    if(!discardUpdateProjectFlag){
        std::string brushFolderPath = destinationPath + UTIL::folderDistinguisher() + "Brushes";
        UTIL::createFolderIfDoesntExist(brushFolderPath);
        PROJECT_updateBrushes(brushFolderPath, this->discardUpdateProjectFlag);
    }
    
    //----------------- Model
    if(!discardUpdateProjectFlag){
        std::string modelFolderPath = destinationPath + UTIL::folderDistinguisher() + "3DModels";
        UTIL::createFolderIfDoesntExist(modelFolderPath);
        PROJECT_updateModels(modelFolderPath, this->discardUpdateProjectFlag);
    }

    //----------------- Ligid
    if(!discardUpdateProjectFlag){

        // Generate the path to the ligid file
        std::string lgdPath = locateLigidFileInFolder(destinationPath); 
        if(lgdPath == ""){
            lgdPath = this->folderPath + UTIL::folderDistinguisher() + this->projectName() + ".ligid";
        }

        // Write the ligid file 
        if(!this->writeLigidFile(lgdPath)){
            if(!multithreadingMode)
                LGDLOG::start << "Saving project folder : Failed to write the ligid file!" << LGDLOG::end; 
            else
                std::cout << "Writing recover files : Failed to write the ligid file!" << std::endl; 
        }
    }

    if(discardUpdateProjectFlag)
        LGDLOG::start << "INFO : Updating project discarded" << LGDLOG::end;
    else
        LGDLOG::start << "Project saved successfuly" << LGDLOG::end;
    
    this->projectProcessing = false;
    this->discardUpdateProjectFlag = false;
}













// ---------------------------- UTILITY FUNCTIONS ---------------------------------


#define PROJECT_UPDATE_PROTOCOL_START(path)     std::string updateFolderPath = path + UTIL::folderDistinguisher() + copyFolderTitle;  \
                                                if(std::filesystem::exists(updateFolderPath)) \
                                                    std::filesystem::remove_all(updateFolderPath); \
                                                if(!std::filesystem::create_directory(updateFolderPath)){ \
                                                    LGDLOG::start << "ERROR : Updating project : faced with an issue while creating the update folder!" << LGDLOG::end; \
                                                    return; \
                                                }

#define PROJECT_UPDATE_PROTOCOL_END(path)   if(!discardUpdateProjectFlag && !error){ \
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

static void PROJECT_updateModels(std::string modelFolderPath, bool& discardUpdateProjectFlag){

    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(modelFolderPath)

    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getModelArraySize(); i++)
    {
        if(!discardUpdateProjectFlag){
            if(!FileHandler::writeLGDMODELFile(updateFolderPath, *Library::getModel(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a model file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(modelFolderPath)
}

static void PROJECT_updateBrushes(std::string brushFolderPath, bool& discardUpdateProjectFlag){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(brushFolderPath)
    
    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getBrushArraySize(); i++)
    {
        if(!discardUpdateProjectFlag){
            if(!FileHandler::writeLGDBRUSHFile(updateFolderPath, Library::getBrushObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a brush file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(brushFolderPath)
}

static void PROJECT_updateMaterials(std::string materialFolderPath, bool& discardUpdateProjectFlag){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(materialFolderPath)

    bool error = false;

    // Write the files to updating folder
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        if(!discardUpdateProjectFlag){
            if(!FileHandler::writeLGDMATERIALFile(updateFolderPath, Library::getMaterialObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a material file!" << LGDLOG::end;
                error = true;
            }
        }
    }

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(materialFolderPath)
}

static void PROJECT_updateTextures(std::string textureFolderPath, bool multithreadingMode, bool& discardUpdateProjectFlag){
    
    // Create updating folder 
    PROJECT_UPDATE_PROTOCOL_START(textureFolderPath)

    // Use the copy OpenGL context
    if(multithreadingMode && !mainThreadUsingCopyContext)
        getCopyContext()->window.makeContextCurrent();
    
    bool error = false;

    // Write the textures
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            // Get the texture
            Texture threadSafeTxtr = Library::getTextureObj(i);
            
            // Get the related texture ID from another OpenGL context If multi-threading mode  
            if(multithreadingMode)
                threadSafeTxtr.ID = threadSafeTxtr.copyContextID;

            // Abort exporting if main thread is using the copy context and we're in the multi-threading mode
            if(mainThreadUsingCopyContext && multithreadingMode){
                error = true;
                LGDLOG::start << "ERROR : Updating project : OpenGL context is shared at the same time" << LGDLOG::end;
                break;
            }

            if(!discardUpdateProjectFlag){
                // Write texture data
                if(!threadSafeTxtr.exportTexture(updateFolderPath, "PNG")){
                    LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a texture file!" << LGDLOG::end;
                    error = true;
                }
            }
        }
    }

    // Release the copy OpenGL context
    if(multithreadingMode && !mainThreadUsingCopyContext)
        getCopyContext()->window.releaseContext();

    // Move the files from updating folder to main folder and remove the updating folder
    PROJECT_UPDATE_PROTOCOL_END(textureFolderPath)
}