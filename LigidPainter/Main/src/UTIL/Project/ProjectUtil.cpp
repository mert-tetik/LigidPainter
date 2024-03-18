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
#include "UTIL/Library/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


std::string Project::locateLigidFileInFolder(const std::string& folderPath)
{
    if(!std::filesystem::is_directory(folderPath) || !std::filesystem::exists(folderPath)){
        LGDLOG::start << "ERROR : Locating Ligid file in a folder : " << folderPath << " is not a folder!"  << LGDLOG::end;
        return "";
    }

    const std::string extension = ".ligid";
    std::vector<std::string> matchingFiles;

    // Get all the files with a ligid extension
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

    // Can't find any ligid files
    if (matchingFiles.empty())
    {
        return "";
    }

    // Ligid files found
    else
    {
        // If multiple ligid files found
        if(matchingFiles.size() > 1)
            LGDLOG::start << "WARNING : Multiple ligid files detected!" << LGDLOG::end;
        
        // Return the first matching file path
        return matchingFiles[0];
    }
}

std::string Project::locateLigidFileInFolder(){
    return this->locateLigidFileInFolder(this->folderPath);
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

    const std::string originalThreedModelsFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Original_3D_Model_Files"; 
    if((UTIL::createFolderIfDoesntExist(originalThreedModelsFolderPath))){
        UTIL::copyFileToFolder(filePath, originalThreedModelsFolderPath, 1);
    }
    
    FileHandler::writeLGDMODELFile(this->folderPath + UTIL::folderDistinguisher() + "3DModels", tdModel);
}   

bool Project::folderPathCheck(){
    // Path doesn't exist
    int tries = 0;
    while (!std::filesystem::exists(this->folderPath))
    {
        // Inform user
        showMessageBox("CRITICAL PROBLEM!", "Your project path is not valid. Please select a new path to your project.", MESSAGEBOX_TYPE_ERROR, MESSAGEBOX_BUTTON_OK);
        tries++;

        // Select new path
        std::string path = showFileSystemObjectSelectionDialog("Compensate the missing project path", UTIL::removeLastWordBySeparatingWithChar(this->absoluteProjectPath(), UTIL::folderDistinguisher()), 0, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
        
        // Try to create the project
        if(std::filesystem::exists(path))
            this->createProject(path, this->projectName(), {});

        // Filesystem has a problem
        if(tries == 3){
            showMessageBox("oopsies", "Appearently LigidPainter & the file system having a problem determining if your project path exists or not. Since this problem is so rare our fellow developer didn't take his time to put a recovery mechanism there specifically. However don't worry! LigidPainter has a recovery system on its own. Just search it up. And you probably need to close the app rn. Sorry :3. Note : Pls contact the developer if u see this (especially Mert Tetik)", MESSAGEBOX_TYPE_ERROR, MESSAGEBOX_BUTTON_OK);
            return false;
        }
    }
    
    return true;
}