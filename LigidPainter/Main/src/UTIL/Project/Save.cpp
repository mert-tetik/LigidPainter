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
#include "LibrarySystem/Library.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define CEATE_FOLDER_IF_NOT_EXISTS(aPath)    if(!std::filesystem::exists(aPath)){ \
                                                if(!std::filesystem::create_directories(aPath)){ \
                                                    LGDLOG::start << "ERROR : Can't write recover files : Can't create the folder : " << aPath << LGDLOG::end; \
                                                    projectProcessing = false; \
                                                    return; \
                                                } \
                                            }         
                                        

static const std::string copyFolderTitle = "Updating_Folder-Remove_if_you_see";

// Forward declared utility functions
static void PROJECT_updateTextures(std::string textureFolderPath, bool multithreadingMode, bool& discardUpdateProjectFlag);
static void PROJECT_updateMaterials(std::string materialFolderPath, bool& discardUpdateProjectFlag);
static void PROJECT_updateBrushes(std::string brushFolderPath, bool& discardUpdateProjectFlag);

// Goes from 1 to 3
static int currentRecoverSlot = 1;

void Project::updateProject(bool updateTextures, bool multithreadingMode){
    
    // Multithreading compat
    while(true){
        if(!this->projectProcessing)
            break;
    }
    this->projectProcessing = true;
    
    // If the project path is not valid
    if(!std::filesystem::exists(folderPath)){
        LGDLOG::start<< "ERROR CAN'T UPDATE THE PROJECT FOLDER : Can't access the project path : " << folderPath << LGDLOG::end;
        this->projectProcessing = false;
        return;
    }
    
    // Process the destination path / Project folder
    std::string destinationPath = this->folderPath;
    if(multithreadingMode){
        destinationPath = this->recoverSlotPath(currentRecoverSlot);   
        currentRecoverSlot++;
        if(currentRecoverSlot == 4)
            currentRecoverSlot = 1;
        CEATE_FOLDER_IF_NOT_EXISTS(destinationPath)
    }

    //----------------- Textures
    if(updateTextures && !discardUpdateProjectFlag){
        std::cout << "UPDATE MODE : TEXTURES" << std::endl;
        std::string textureFolderPath = destinationPath + UTIL::folderDistinguisher() + "Textures";
        CEATE_FOLDER_IF_NOT_EXISTS(textureFolderPath)
        PROJECT_updateTextures(textureFolderPath, multithreadingMode, this->discardUpdateProjectFlag);
    }
    
    //----------------- Materials
    if(!discardUpdateProjectFlag){
        std::cout << "UPDATE MODE : MATERIALS" << std::endl;

        std::string materialFolderPath = destinationPath + UTIL::folderDistinguisher() + "Materials";
        CEATE_FOLDER_IF_NOT_EXISTS(materialFolderPath)
        PROJECT_updateMaterials(materialFolderPath, this->discardUpdateProjectFlag);
    }

    //----------------- Brushes
    if(!discardUpdateProjectFlag){
        std::cout << "UPDATE MODE : BRUSHES" << std::endl;

        std::string brushFolderPath = destinationPath + UTIL::folderDistinguisher() + "Brushes";
        CEATE_FOLDER_IF_NOT_EXISTS(brushFolderPath)
        PROJECT_updateBrushes(brushFolderPath, this->discardUpdateProjectFlag);
    }

    //----------------- Ligid
    if(!discardUpdateProjectFlag){
        std::cout << "UPDATE MODE : LIGID" << std::endl;
        std::string lgdPath;
        locateLigidFileInFolder(destinationPath, lgdPath); 
        if(!this->writeLigidFile(lgdPath)){
            if(!multithreadingMode)
                LGDLOG::start << "Saving project folder : Failed to write the ligid file!" << LGDLOG::end; 
            else
                std::cout << "Writing recover files : Failed to write the ligid file!" << std::endl; 
        }
    }

    if(discardUpdateProjectFlag){
        LGDLOG::start << "INFO : Updating project discarded" << LGDLOG::end;
    }

    this->projectProcessing = false;
    this->discardUpdateProjectFlag = false;

    std::cout << "UPDATE MODE : FINISH" << std::endl;
}













// ---------------------------- UTILITY FUNCTIONS ---------------------------------
static void PROJECT_updateBrushes(std::string brushFolderPath, bool& discardUpdateProjectFlag){
    std::string updateBrushFolderPath = brushFolderPath + UTIL::folderDistinguisher() + copyFolderTitle;

    if(std::filesystem::exists(updateBrushFolderPath))
        std::filesystem::remove_all(updateBrushFolderPath);

    if(!std::filesystem::create_directory(updateBrushFolderPath)){
        LGDLOG::start << "ERROR : Updating project : faced with an issue while creating the update folder!" << LGDLOG::end;
        return;
    }

    //Write the brushes
    for (size_t i = 0; i < Library::getBrushArraySize(); i++)
    {
        //Export brush
        if(!discardUpdateProjectFlag){
            if(!FileHandler::writeLGDBRUSHFile(updateBrushFolderPath, Library::getBrushObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a brush file!" << LGDLOG::end;
            }
        }
    }

    if(!discardUpdateProjectFlag){
        if(UTIL::deleteFilesInFolder(brushFolderPath)){
            UTIL::moveFilesToDestination(updateBrushFolderPath, brushFolderPath);
            std::filesystem::remove_all(updateBrushFolderPath);
        }
        else{
            LGDLOG::start << "WARNING! : Updating project : faced with an issue while deleting the brush folder" << LGDLOG::end;
        }
    }
    else{
        std::filesystem::remove_all(updateBrushFolderPath);
    }
}

static void PROJECT_updateMaterials(std::string materialFolderPath, bool& discardUpdateProjectFlag){
    std::string updateMaterialFolderPath = materialFolderPath + UTIL::folderDistinguisher() + copyFolderTitle;

    if(std::filesystem::exists(updateMaterialFolderPath))
        std::filesystem::remove_all(updateMaterialFolderPath);

    if(!std::filesystem::create_directory(updateMaterialFolderPath)){
        LGDLOG::start << "ERROR : Updating project : faced with an issue while creating the update folder!" << LGDLOG::end;
        return;
    }

    //Write the materials
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        //Export material
        if(!discardUpdateProjectFlag){
            if(!FileHandler::writeLGDMATERIALFile(updateMaterialFolderPath + UTIL::folderDistinguisher() + Library::getMaterialObj(i).title + ".lgdmaterial", Library::getMaterialObj(i))){
                LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a material file!" << LGDLOG::end;
            }
        }
    }

    if(!discardUpdateProjectFlag){
        if(UTIL::deleteFilesInFolder(materialFolderPath)){
            UTIL::moveFilesToDestination(updateMaterialFolderPath, materialFolderPath);
            std::filesystem::remove_all(updateMaterialFolderPath);
        }
        else{
            LGDLOG::start << "WARNING! : Updating project : faced with an issue while deleting the material folder" << LGDLOG::end;
        }
    }
    else{
        std::filesystem::remove_all(updateMaterialFolderPath);
    }
}

static void PROJECT_updateTextures(std::string textureFolderPath, bool multithreadingMode, bool& discardUpdateProjectFlag){
    
    // Folder path to write the textures in it
    std::string updateTextureFolderPath = textureFolderPath + UTIL::folderDistinguisher() + copyFolderTitle;

    // Remove the previous one
    if(std::filesystem::exists(updateTextureFolderPath))
        std::filesystem::remove_all(updateTextureFolderPath);

    // Create the texture writing folder from scratch
    if(!std::filesystem::create_directory(updateTextureFolderPath)){
        LGDLOG::start << "ERROR : Updating project : faced with an issue while creating the update folder!" << LGDLOG::end;
        return;
    }

    // Use the copy OpenGL context
    if(multithreadingMode && !mainThreadUsingCopyContext)
        getCopyContext()->window.makeContextCurrent();
    
    bool error = false;

    // Write the textures
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            Texture threadSafeTxtr = Library::getTextureObj(i);
            if(multithreadingMode)
                threadSafeTxtr.ID = threadSafeTxtr.copyContextID;

            if(mainThreadUsingCopyContext && multithreadingMode){
                error = true;
                LGDLOG::start << "ERROR : Updating project : OpenGL context is shared at the same time" << LGDLOG::end;
                break;
            }

            if(!discardUpdateProjectFlag){
                if(!threadSafeTxtr.exportTexture(updateTextureFolderPath, "PNG")){
                    LGDLOG::start << "WARNING! : Updating project : faced with an issue while writing a texture file!" << LGDLOG::end;
                    error = true;
                }
            }
        }
    }

    // Release the copy OpenGL context
    if(multithreadingMode && !mainThreadUsingCopyContext)
        getCopyContext()->window.releaseContext();

    // If there are no errors move the textures inside of the updating folder to the Textures folder
    if(!error && !discardUpdateProjectFlag){
        if(UTIL::deleteFilesInFolder(textureFolderPath)){
            UTIL::moveFilesToDestination(updateTextureFolderPath, textureFolderPath);
            std::filesystem::remove_all(updateTextureFolderPath);
        }
        else{
            LGDLOG::start << "WARNING! : Updating project : faced with an issue while deleting the texture folder" << LGDLOG::end;
        }
    }
    else{
        std::filesystem::remove_all(updateTextureFolderPath);
    }
}