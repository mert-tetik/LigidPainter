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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


bool Project::locateLigidFileInFolder(const std::string& folderPath, std::string& ligidPath)
{
    const std::string extension = ".ligid";
    std::vector<std::string> matchingFiles;

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
            {
                matchingFiles.push_back(entry.path().string());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 789215 " << ex.what() << LGDLOG::end;
    }    

    if (matchingFiles.empty())
    {
        ligidPath = this->folderPath + UTIL::folderDistinguisher() + this->projectName() + ".ligid"; // No file with the given extension found
        return false;
    }
    else
    {
        // Return the first matching file path
        ligidPath = matchingFiles[0];
        if(matchingFiles.size() > 1)
            LGDLOG::start << "WARNING : Multiple ligid files detected!" << LGDLOG::end;
        return true;
    }
}

std::string Project::absoluteProjectPath(){
    std::string res = std::filesystem::absolute(this->folderPath).string();
    UTIL::correctFolderDistinguishers(res);
    return res;
}

std::string Project::recoverSlotPath(int slot){
    return this->folderPath + UTIL::folderDistinguisher() + "Recover" + UTIL::folderDistinguisher() + std::to_string(slot) + UTIL::folderDistinguisher();
}

void Project::copyTheProjectPathToTheClipboard(){
    LigidGL::setClipboardText(this->absoluteProjectPath().c_str());
}

std::string Project::ligidFilePath(){
    std::string a;
    this->locateLigidFileInFolder(this->folderPath, a);
    return a;
}

std::string Project::projectName(){
    return UTIL::getLastWordBySeparatingWithChar(this->folderPath, UTIL::folderDistinguisher());
}

void Project::addModelToProject(std::string filePath){
    Model tdModel;
    bool success = tdModel.loadModel(filePath, true, false);
    
    if(!success){
        LGDLOG::start << "ERROR : Can't add the 3D model to the library. Failed to read the 3D model file : " << filePath << LGDLOG::end;
        return;
    }
    
    if(!tdModel.meshes.size()){
        LGDLOG::start << "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << LGDLOG::end;
        return;
    }
   
    Library::addModel(tdModel);
    UTIL::copyFileToFolder(filePath, this->folderPath + UTIL::folderDistinguisher() + "3DModels", 1);

    FileHandler::writeLGDMODELFile(this->folderPath + UTIL::folderDistinguisher() + "3DModels", tdModel);
}   