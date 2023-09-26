/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO UPDATE LIGID FILE DATE

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

void Project::updateProject(bool updateTextures, bool multithreadingMode){
    
    while(true){
        if(!this->projectProcessing)
            break;
    }

    this->projectProcessing = true;

    if(!std::filesystem::exists(folderPath)){
        LGDLOG::start<< "ERROR CAN'T UPDATE THE PROJECT FOLDER : " << this->folderPath << LGDLOG::end;
        return;
    }


    if(updateTextures){
        //!Textures
        std::string textureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures";
        std::string updateTextureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures" + UTIL::folderDistinguisher() + copyFolderTitle;

        if(std::filesystem::exists(updateTextureFolderPath))
            std::filesystem::remove_all(updateTextureFolderPath);

        std::filesystem::create_directory(updateTextureFolderPath);

        if(multithreadingMode)
            getCopyContext()->window.makeContextCurrent();
        
        //Write the textures
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            if(i < Library::getTextureArraySize()){
                Texture threadSafeTxtr = Library::getTextureObj(i);
                if(multithreadingMode)
                    threadSafeTxtr.ID = threadSafeTxtr.copyContextID;

                threadSafeTxtr.exportTexture(updateTextureFolderPath, "PNG");
            }
        }
    
        if(multithreadingMode)
            getCopyContext()->window.releaseContext();

        if(!UTIL::deleteFilesInFolder(textureFolderPath)){
            return;
        }

        UTIL::moveFilesToDestination(updateTextureFolderPath, textureFolderPath);
        std::filesystem::remove_all(updateTextureFolderPath);
    }



    
    //!Materials
    std::string materialFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials";
    std::string updateMaterialFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials" + UTIL::folderDistinguisher() + copyFolderTitle;

    if(std::filesystem::exists(updateMaterialFolderPath))
        std::filesystem::remove_all(updateMaterialFolderPath);

    std::filesystem::create_directory(updateMaterialFolderPath);

    //Write the materials
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        //Export material
        FileHandler::writeLGDMATERIALFile(updateMaterialFolderPath + UTIL::folderDistinguisher() + Library::getMaterialObj(i).title + ".lgdmaterial", Library::getMaterialObj(i));
    }

    if(!UTIL::deleteFilesInFolder(materialFolderPath)){
        return;
    }

    UTIL::moveFilesToDestination(updateMaterialFolderPath, materialFolderPath);
    std::filesystem::remove_all(updateMaterialFolderPath);




    //!Brushes
    std::string brushFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes";
    std::string updateBrushFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes" + UTIL::folderDistinguisher() + copyFolderTitle;

    if(std::filesystem::exists(updateBrushFolderPath))
        std::filesystem::remove_all(updateBrushFolderPath);

    std::filesystem::create_directory(updateBrushFolderPath);

    //Write the brushes
    for (size_t i = 0; i < Library::getBrushArraySize(); i++)
    {
        //Export brush
        FileHandler::writeLGDBRUSHFile(updateBrushFolderPath, Library::getBrushObj(i));
    }

    if(!UTIL::deleteFilesInFolder(brushFolderPath)){
        return;
    }

    UTIL::moveFilesToDestination(updateBrushFolderPath, brushFolderPath);
    std::filesystem::remove_all(updateBrushFolderPath);





    
    //!3D Models
    std::string tdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels";
    std::string updateTdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels" + UTIL::folderDistinguisher() + copyFolderTitle;

    if(std::filesystem::exists(updateTdModelFolderPath))
        std::filesystem::remove_all(updateTdModelFolderPath);

    std::filesystem::create_directory(updateTdModelFolderPath);

    //Write the 3D models
    for (size_t i = 0; i < Library::getModelArraySize(); i++)
    {
        //Export 3D model
        FileHandler::writeOBJFile(updateTdModelFolderPath + UTIL::folderDistinguisher() + Library::getModelObj(i).title + ".obj", Library::getModelObj(i));
    }

    if(!UTIL::deleteFilesInFolder(tdModelFolderPath)){
        return;
    }

    UTIL::moveFilesToDestination(updateTdModelFolderPath, tdModelFolderPath);
    std::filesystem::remove_all(updateTdModelFolderPath);

    this->writeLigidFile();

    this->projectProcessing = false;
}