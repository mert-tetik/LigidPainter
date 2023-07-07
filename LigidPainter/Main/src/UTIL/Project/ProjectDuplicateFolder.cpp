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

    if(dstPath == ""){
        dstPath = showFileSystemObjectSelectionDialog("Select a folder to duplicate the project folder file.", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }

    if(!dstPath.size())
        return;

    //Remove the / or \ at the end of the destination path if there are any
    if(dstPath.size()){
        if(dstPath[dstPath.size() - 1] == '/' || dstPath[dstPath.size() - 1] == '\\'){
            dstPath.pop_back();
        }
    }


    //If the destination path doesn't exists
    if(!std::filesystem::exists(dstPath)){
        std::cout << "ERROR : Can't duplicate the project folder! Destination path doesn't exists : " << dstPath << std::endl;
        return;
    }

    // DestinationPath/MyProject
    std::string dstProjectPath = dstPath + UTIL::folderDistinguisher() + this->projectName();

    //If there is already a folder with that name
    if(std::filesystem::exists(dstProjectPath)){
        std::cout << "ERROR : Can't duplicate the project folder! There is already a folder with the same name : " << dstPath << std::endl;
        return;
    }

    //Duplicate files and folders from the original path to destination path
    UTIL::duplicateFolder(this->folderPath, dstProjectPath);

    //Update the project path
    this->folderPath = dstProjectPath;
}