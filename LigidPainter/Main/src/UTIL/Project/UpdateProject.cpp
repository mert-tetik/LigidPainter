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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


void Project::updateProject(){
    
    if(!std::filesystem::exists(folderPath)){
        LGDLOG::start<< "ERROR CAN'T UPDATE THE PROJECT FOLDER : " << this->folderPath << LGDLOG::end;
        return;
    }
    
    //!Textures
    std::string textureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures";

    //Clear the textures folder
    UTIL::deleteFilesInFolder(textureFolderPath);

    //Write the textures
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        //Export texture
        Library::getTexture(i)->exportTexture(textureFolderPath, "PNG");
    }
    
    
    //!Materials
    std::string materialFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials";

    //Clear the materials folder
    UTIL::deleteFilesInFolder(materialFolderPath);

    //Write the materials
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        //Export material
        FileHandler::writeLGDMATERIALFile(materialFolderPath + UTIL::folderDistinguisher() + Library::getMaterial(i)->title + ".lgdmaterial", *Library::getMaterial(i));
    }


    //!Brushes
    std::string brushFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes";

    //Clear the brushes folder
    UTIL::deleteFilesInFolder(brushFolderPath);

    //Write the brushes
    for (size_t i = 0; i < Library::getBrushArraySize(); i++)
    {
        //Export brush
        FileHandler::writeLGDBRUSHFile(brushFolderPath, *Library::getBrush(i));
    }
    
    //!3D Models
    std::string tdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels";

    //Clear the 3D models folder
    UTIL::deleteFilesInFolder(tdModelFolderPath);

    //Write the 3D models
    for (size_t i = 0; i < Library::getModelArraySize(); i++)
    {
        //Export 3D model
        FileHandler::writeOBJFile(tdModelFolderPath + UTIL::folderDistinguisher() + Library::getModel(i)->title + ".obj", *Library::getModel(i));
    }

    writeLigidFile();
}