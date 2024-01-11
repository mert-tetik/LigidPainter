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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


void Project::duplicateFolder(std::string dstPath)
{
    LGDLOG::start << "WARNING! Saving as feature for the project folder is disabled for that version!" << LGDLOG::end;
    return;
    
    // Save the project before duplicating the project folder    
    this->updateProject(true, false);
    
    while(true){
        if(!this->projectProcessing)
            break;
    }

    this->projectProcessing = true;

    if(dstPath == ""){
        dstPath = showFileSystemObjectSelectionDialog("Select a folder to duplicate the project folder file.", UTIL::removeLastWordBySeparatingWithChar(this->absoluteProjectPath(), UTIL::folderDistinguisher()), {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }

    if(!dstPath.size()){
        this->projectProcessing = false;
        return;
    }

    //Remove the / or \ at the end of the destination path if there are any
    if(dstPath.size()){
        if(dstPath[dstPath.size() - 1] == '/' || dstPath[dstPath.size() - 1] == '\\'){
            dstPath.pop_back();
        }
    }

    //If the destination path doesn't exists
    if(!std::filesystem::exists(dstPath)){
        LGDLOG::start<< "ERROR : Can't duplicate the project folder! Destination path doesn't exists : " << dstPath << LGDLOG::end;
        this->projectProcessing = false;
        return;
    }

    // DestinationPath/MyProject
    std::string dstProjectPath = dstPath + UTIL::folderDistinguisher() + this->projectName();

    while (std::filesystem::exists(dstProjectPath))
    {
        dstProjectPath.push_back('_');
    }

    //Duplicate files and folders from the original path to destination path
    UTIL::duplicateFolder(this->folderPath, dstProjectPath);

    //Update the project path
    this->folderPath = dstProjectPath;

    this->projectProcessing = false;
    return;
}